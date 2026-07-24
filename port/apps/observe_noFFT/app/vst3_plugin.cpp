#include "vst3_plugin.h"

#include <algorithm>
#include <cstring>
#include <pluginterfaces/vst/ivsteditcontroller.h>

// ---------------------------------------------------------------------------
// TEMPORARY DPI DIAGNOSTICS -- remove once the Ableton scaling issue is fixed.
// Logs the exact VST3 host<->plugin size/scale call sequence via
// OutputDebugStringA so it is visible in DebugView (Sysinternals) even in a
// Release build. Set to 0 to disable.
// ---------------------------------------------------------------------------
#define VST3_DPI_DEBUG_LOG 1
#if VST3_DPI_DEBUG_LOG && defined(_WIN32)
#include <cstdio>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#define VST3_DPI_LOG(...)                                                              \
  do                                                                                   \
  {                                                                                    \
    char _vst3_dpi_buf[512];                                                           \
    std::snprintf(_vst3_dpi_buf, sizeof(_vst3_dpi_buf), "[observe.vst3] " __VA_ARGS__); \
    OutputDebugStringA(_vst3_dpi_buf);                                                 \
    OutputDebugStringA("\n");                                                          \
  } while (0)
#else
#define VST3_DPI_LOG(...) ((void)0)
#endif

using namespace Steinberg;
using namespace Steinberg::Vst;
using namespace observe::dimension;

namespace
{

float readSample(const AudioBusBuffers &input, int32 channel, int32 frame, int32 symbolic_sample_size)
{
  if (symbolic_sample_size == kSample64 && input.channelBuffers64 != nullptr)
    return static_cast<float>(input.channelBuffers64[channel][frame]);
  if (input.channelBuffers32 != nullptr)
    return input.channelBuffers32[channel][frame];
  return 0.0f;
}

void writeSample(AudioBusBuffers &output, int32 channel, int32 frame, int32 symbolic_sample_size, float sample)
{
  if (symbolic_sample_size == kSample64 && output.channelBuffers64 != nullptr)
    output.channelBuffers64[channel][frame] = sample;
  else if (output.channelBuffers32 != nullptr)
    output.channelBuffers32[channel][frame] = sample;
}

void copyOrClearOutput(ProcessData &data)
{
  if (data.numOutputs < 1)
    return;

  AudioBusBuffers &output = data.outputs[0];
  AudioBusBuffers *input = data.numInputs > 0 ? &data.inputs[0] : nullptr;

  for (int32 ch = 0; ch < output.numChannels; ++ch)
  {
    const int32 input_ch = input != nullptr && input->numChannels > 0 ? std::min(ch, input->numChannels - 1) : 0;
    for (int32 frame = 0; frame < data.numSamples; ++frame)
    {
      const float sample = input != nullptr ? readSample(*input, input_ch, frame, data.symbolicSampleSize) : 0.0f;
      writeSample(output, ch, frame, data.symbolicSampleSize, sample);
    }
  }
}

} // namespace

OscillatorsVst3PlugView::OscillatorsVst3PlugView(OscillatorsVst3Plugin *controller) : controller_(controller) {}

OscillatorsVst3PlugView::~OscillatorsVst3PlugView() = default;

int OscillatorsVst3PlugView::pluginWidth() const
{
  if (app_ == nullptr)
    return 0;

#if __APPLE__
  return app_->width();
#else
  return app_->nativeWidth();
#endif
}

int OscillatorsVst3PlugView::pluginHeight() const
{
  if (app_ == nullptr)
    return 0;

#if __APPLE__
  return app_->height();
#else
  return app_->nativeHeight();
#endif
}

void OscillatorsVst3PlugView::setPluginDimensions(int width, int height)
{
  if (app_ == nullptr)
    return;

#if __APPLE__
  app_->setWindowDimensions(width, height);
#else
  app_->setNativeWindowDimensions(width, height);
#endif
}

void OscillatorsVst3PlugView::requestResizeFromHost()
{
  if (plug_frame_ == nullptr || app_ == nullptr)
    return;

  ViewRect rect(0, 0, pluginWidth(), pluginHeight());
  VST3_DPI_LOG("requestResizeFromHost() -> resizeView %dx%d", rect.getWidth(), rect.getHeight());
  plug_frame_->resizeView(this, &rect);
}

tresult PLUGIN_API OscillatorsVst3PlugView::isPlatformTypeSupported(FIDString type)
{
#ifdef _WIN32
  if (std::strcmp(type, kPlatformTypeHWND) == 0)
    return kResultTrue;
#elif __APPLE__
  if (std::strcmp(type, kPlatformTypeNSView) == 0)
    return kResultTrue;
#elif __linux__
  if (std::strcmp(type, kPlatformTypeX11EmbedWindowID) == 0)
    return kResultTrue;
#endif

  return kResultFalse;
}

tresult PLUGIN_API OscillatorsVst3PlugView::attached(void *parent, FIDString type)
{
  VST3_DPI_LOG("attached() type=%s parent=%p content_scale=%.3f", type ? type : "(null)",
               parent, content_scale_);
  if (isPlatformTypeSupported(type) != kResultTrue || controller_ == nullptr)
    return kResultFalse;

  if (app_ == nullptr)
  {
    app_ = std::make_unique<observe::ApplicationWindow>();
    app_->setWindowDimensions(100_vmin, 60_vmin);

    spectrum_ = std::make_unique<oscillators_observe::SpectrumView>(*app_, controller_->model(), false);
    spectrum_->install();

    app_->onWindowContentsResized() = [this] { requestResizeFromHost(); };
  }

  app_->show(parent);
  VST3_DPI_LOG("attached() after show: native=%dx%d logical=%.1fx%.1f dpiScale=%.3f",
               app_->nativeWidth(), app_->nativeHeight(), app_->width(), app_->height(),
               app_->dpiScale());
  return kResultTrue;
}

tresult PLUGIN_API OscillatorsVst3PlugView::removed()
{
  if (app_)
    app_->close();
  spectrum_ = nullptr;
  app_ = nullptr;
  plug_frame_ = nullptr;
  return kResultTrue;
}

tresult PLUGIN_API OscillatorsVst3PlugView::getSize(ViewRect *size)
{
  if (app_ == nullptr || size == nullptr)
  {
    VST3_DPI_LOG("getSize() called with app_=%p size=%p -> kResultFalse", (void *)app_.get(),
                 (void *)size);
    return kResultFalse;
  }

  size->left = 0;
  size->top = 0;
  size->right = static_cast<int32>(pluginWidth());
  size->bottom = static_cast<int32>(pluginHeight());
  VST3_DPI_LOG("getSize() -> %dx%d (native=%dx%d dpiScale=%.3f)", size->getWidth(),
               size->getHeight(), app_->nativeWidth(), app_->nativeHeight(), app_->dpiScale());
  return kResultTrue;
}

tresult PLUGIN_API OscillatorsVst3PlugView::onSize(ViewRect *newSize)
{
  if (app_ == nullptr || newSize == nullptr)
    return kResultFalse;

  VST3_DPI_LOG("onSize() incoming %dx%d (before: native=%dx%d dpiScale=%.3f)",
               newSize->getWidth(), newSize->getHeight(), app_->nativeWidth(),
               app_->nativeHeight(), app_->dpiScale());
  setPluginDimensions(newSize->getWidth(), newSize->getHeight());
  VST3_DPI_LOG("onSize() after: native=%dx%d logical=%.1fx%.1f dpiScale=%.3f",
               app_->nativeWidth(), app_->nativeHeight(), app_->width(), app_->height(),
               app_->dpiScale());
  return kResultTrue;
}

tresult PLUGIN_API OscillatorsVst3PlugView::setFrame(IPlugFrame *frame)
{
  VST3_DPI_LOG("setFrame(%p)", (void *)frame);
  plug_frame_ = frame;
  return kResultTrue;
}

tresult PLUGIN_API OscillatorsVst3PlugView::canResize()
{
  return kResultTrue;
}

tresult PLUGIN_API OscillatorsVst3PlugView::checkSizeConstraint(ViewRect *rect)
{
  if (app_ == nullptr || rect == nullptr)
    return kResultFalse;

  auto width = static_cast<uint32_t>(rect->getWidth());
  auto height = static_cast<uint32_t>(rect->getHeight());
  VST3_DPI_LOG("checkSizeConstraint() in %ux%u", width, height);
  app_->adjustWindowDimensions(&width, &height, true, true);
  rect->right = rect->left + static_cast<int32>(width);
  rect->bottom = rect->top + static_cast<int32>(height);
  VST3_DPI_LOG("checkSizeConstraint() out %ux%u", width, height);
  return kResultTrue;
}

tresult PLUGIN_API OscillatorsVst3PlugView::setContentScaleFactor(ScaleFactor factor)
{
  VST3_DPI_LOG("setContentScaleFactor(%.4f) app_=%p", factor, (void *)app_.get());
#if __APPLE__
  // On macOS the ViewRect is already in logical NSView points and the OS
  // handles backing-store scaling, so there is nothing to acknowledge.
  return kResultFalse;
#else
  // Store the host-reported factor and acknowledge support. Returning
  // kResultTrue is what flips DPI-aware Windows hosts into treating our
  // ViewRect as physical pixels (matching how observe reports and renders).
  // observe derives its own dpi scale from the OS, so we don't apply the
  // factor to any geometry here -- doing so would double-scale the window.
  content_scale_ = factor;
  return kResultTrue;
#endif
}

OscillatorsVst3Plugin::OscillatorsVst3Plugin() = default;

tresult PLUGIN_API OscillatorsVst3Plugin::initialize(FUnknown *context)
{
  tresult result = SingleComponentEffect::initialize(context);
  if (result != kResultOk)
    return result;

  addAudioInput(STR16("Input"), SpeakerArr::kStereo);
  addAudioOutput(STR16("Output"), SpeakerArr::kStereo);
  return kResultOk;
}

tresult PLUGIN_API OscillatorsVst3Plugin::terminate()
{
  return SingleComponentEffect::terminate();
}

tresult PLUGIN_API OscillatorsVst3Plugin::setupProcessing(ProcessSetup &setup)
{
  model_.setSampleRate(setup.sampleRate);
  return SingleComponentEffect::setupProcessing(setup);
}

tresult PLUGIN_API OscillatorsVst3Plugin::setBusArrangements(SpeakerArrangement *inputs,
                                                             int32 numIns,
                                                             SpeakerArrangement *outputs,
                                                             int32 numOuts)
{
  if (numIns != 1 || inputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  if (numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return kResultTrue;
}

tresult PLUGIN_API OscillatorsVst3Plugin::process(ProcessData &data)
{
  copyOrClearOutput(data);

  if (data.numInputs < 1 || data.inputs[0].numChannels < 1)
    return kResultOk;

  const AudioBusBuffers &input = data.inputs[0];
  mono_buffer_.resize(data.numSamples);

  for (int32 frame = 0; frame < data.numSamples; ++frame)
  {
    float sample = 0.0f;
    for (int32 ch = 0; ch < input.numChannels; ++ch)
      sample += readSample(input, ch, frame, data.symbolicSampleSize);
    mono_buffer_[frame] = sample / static_cast<float>(input.numChannels);
  }

  model_.processBlock(mono_buffer_.data(), static_cast<int>(mono_buffer_.size()));
  return kResultOk;
}

IPlugView *PLUGIN_API OscillatorsVst3Plugin::createView(FIDString name)
{
  if (name && std::strcmp(name, ViewType::kEditor) == 0)
    return new OscillatorsVst3PlugView(this);

  return nullptr;
}
