#include "vst3_plugin.h"

#include <algorithm>
#include <cstring>
#include <pluginterfaces/vst/ivsteditcontroller.h>

using namespace Steinberg;
using namespace Steinberg::Vst;
using namespace observe::dimension;

//------------------------------------------------------------------------
// Vst3PlugView
//------------------------------------------------------------------------

Vst3PlugView::Vst3PlugView(Vst3Plugin* controller) : controller_(controller) { }

Vst3PlugView::~Vst3PlugView() = default;

int Vst3PlugView::pluginWidth() const {
  if (app_ == nullptr)
    return 0;

#if __APPLE__
  return app_->width();
#else
  return app_->nativeWidth();
#endif
}

int Vst3PlugView::pluginHeight() const {
  if (app_ == nullptr)
    return 0;

#if __APPLE__
  return app_->height();
#else
  return app_->nativeHeight();
#endif
}

void Vst3PlugView::setPluginDimensions(int width, int height) {
  if (app_ == nullptr)
    return;

#if __APPLE__
  app_->setWindowDimensions(width, height);
#else
  app_->setNativeWindowDimensions(width, height);
#endif
}

void Vst3PlugView::requestResizeFromHost() {
  if (plug_frame_ == nullptr || app_ == nullptr)
    return;

  ViewRect rect(0, 0, pluginWidth(), pluginHeight());
  plug_frame_->resizeView(this, &rect);
}

tresult PLUGIN_API Vst3PlugView::isPlatformTypeSupported(FIDString type) {
#ifdef _WIN32
  if (strcmp(type, kPlatformTypeHWND) == 0)
    return kResultTrue;
#elif __APPLE__
  if (strcmp(type, kPlatformTypeNSView) == 0)
    return kResultTrue;
#elif __linux__
  if (strcmp(type, kPlatformTypeX11EmbedWindowID) == 0)
    return kResultTrue;
#endif

  return kResultFalse;
}

tresult PLUGIN_API Vst3PlugView::attached(void* parent, FIDString type) {
  if (isPlatformTypeSupported(type) != kResultTrue)
    return kResultFalse;

  if (app_ == nullptr) {
    app_ = std::make_unique<observe::ApplicationWindow>();
    app_->setWindowDimensions(80_vmin, 60_vmin);

    app_->onDraw() = [this](observe::Canvas& canvas) {
      canvas.setColor(0xff000066);
      canvas.fill(0, 0, app_->width(), app_->height());

      float circle_radius = app_->height() * 0.1f;
      float x = app_->width() * 0.5f - circle_radius;
      float y = app_->height() * 0.5f - circle_radius;
      canvas.setColor(0xff00ffff);
      canvas.circle(x, y, 2.0f * circle_radius);
    };

    app_->onWindowContentsResized() = [this] { requestResizeFromHost(); };
  }

  app_->show(parent);
  return kResultTrue;
}

tresult PLUGIN_API Vst3PlugView::removed() {
  if (app_) {
    app_->close();
    app_ = nullptr;
  }
  plug_frame_ = nullptr;
  return kResultTrue;
}

tresult PLUGIN_API Vst3PlugView::getSize(ViewRect* size) {
  if (app_ == nullptr || size == nullptr)
    return kResultFalse;

  size->left = 0;
  size->top = 0;
  size->right = static_cast<int32>(pluginWidth());
  size->bottom = static_cast<int32>(pluginHeight());
  return kResultTrue;
}

tresult PLUGIN_API Vst3PlugView::onSize(ViewRect* newSize) {
  if (app_ == nullptr || newSize == nullptr)
    return kResultFalse;

  setPluginDimensions(newSize->getWidth(), newSize->getHeight());
  return kResultTrue;
}

tresult PLUGIN_API Vst3PlugView::setFrame(IPlugFrame* frame) {
  plug_frame_ = frame;
  return kResultTrue;
}

tresult PLUGIN_API Vst3PlugView::canResize() {
  return kResultTrue;
}

tresult PLUGIN_API Vst3PlugView::checkSizeConstraint(ViewRect* rect) {
  if (app_ == nullptr || rect == nullptr)
    return kResultFalse;

  auto width = static_cast<uint32_t>(rect->getWidth());
  auto height = static_cast<uint32_t>(rect->getHeight());
  app_->adjustWindowDimensions(&width, &height, true, true);
  rect->right = rect->left + static_cast<int32>(width);
  rect->bottom = rect->top + static_cast<int32>(height);
  return kResultTrue;
}

tresult PLUGIN_API Vst3PlugView::setContentScaleFactor(ScaleFactor factor) {
  (void)factor;
#if __APPLE__
  // On macOS the ViewRect is already in logical NSView points and the OS
  // handles backing-store scaling, so there is nothing to acknowledge.
  return kResultFalse;
#else
  // Acknowledge support so DPI-aware Windows hosts treat our ViewRect as
  // physical pixels, matching how observe reports and renders. observe derives
  // its own dpi scale from the OS, so applying the factor here would
  // double-scale the window.
  // VST3 requires resizeView() after accepting a new content scale.
  requestResizeFromHost();
  return kResultTrue;
#endif
}

//------------------------------------------------------------------------
// Vst3Plugin
//------------------------------------------------------------------------

Vst3Plugin::Vst3Plugin() = default;

tresult PLUGIN_API Vst3Plugin::initialize(FUnknown* context) {
  tresult result = SingleComponentEffect::initialize(context);
  if (result != kResultOk)
    return result;

  // add audio buses here with addAudioInput()/addAudioOutput() (inherited from SingleComponentEffect).
  addAudioInput(STR16("Input"), SpeakerArr::kStereo);
  addAudioOutput(STR16("Output"), SpeakerArr::kStereo);
  return kResultOk;
}

tresult PLUGIN_API Vst3Plugin::terminate() {
  return SingleComponentEffect::terminate();
}

tresult PLUGIN_API Vst3Plugin::setBusArrangements(SpeakerArrangement* inputs, int32 numIns,
                                                  SpeakerArrangement* outputs, int32 numOuts) {
  if (numIns != 1 || inputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  if (numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return kResultTrue;
}

tresult PLUGIN_API Vst3Plugin::process(ProcessData& data) {
  if (data.numInputs < 1 || data.numOutputs < 1)
    return kResultOk;

  AudioBusBuffers& in = data.inputs[0];
  AudioBusBuffers& out = data.outputs[0];
  int32 numChannels = std::min(in.numChannels, out.numChannels);

  for (int32 ch = 0; ch < numChannels; ++ch) {
    if (in.channelBuffers32[ch] != out.channelBuffers32[ch])
      std::memcpy(out.channelBuffers32[ch], in.channelBuffers32[ch], sizeof(float) * data.numSamples);
  }

  // Zero out any extra output channels the input doesn't cover.
  for (int32 ch = numChannels; ch < out.numChannels; ++ch)
    std::memset(out.channelBuffers32[ch], 0, sizeof(float) * data.numSamples);

  return kResultOk;
}

IPlugView* PLUGIN_API Vst3Plugin::createView(FIDString name) {
  if (name && strcmp(name, ViewType::kEditor) == 0)
    return new Vst3PlugView(this);

  return nullptr;
}
