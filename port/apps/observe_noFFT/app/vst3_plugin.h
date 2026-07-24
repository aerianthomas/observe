#pragma once

#include <base/source/fobject.h>
#include <memory>
#include <pluginterfaces/gui/iplugviewcontentscalesupport.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <public.sdk/source/vst/vstsinglecomponenteffect.h>
#include <observe/app.h>
#include <vector>

#include "resonate_view.h"

static const Steinberg::FUID kOscillatorsVst3ProcessorUID(0xB0D7C3C6, 0x641D463D, 0xBF6B72B5, 0x3096E76A);

class OscillatorsVst3Plugin;

class OscillatorsVst3PlugView : public Steinberg::FObject,
                                public Steinberg::IPlugView,
                                public Steinberg::IPlugViewContentScaleSupport
{
public:
  explicit OscillatorsVst3PlugView(OscillatorsVst3Plugin *controller);
  ~OscillatorsVst3PlugView() override;

  Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) override;
  Steinberg::tresult PLUGIN_API attached(void *parent, Steinberg::FIDString type) override;
  Steinberg::tresult PLUGIN_API removed() override;
  Steinberg::tresult PLUGIN_API onWheel(float distance) override { return Steinberg::kResultFalse; }
  Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key,
                                          Steinberg::int16 keyCode,
                                          Steinberg::int16 modifiers) override { return Steinberg::kResultFalse; }
  Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key,
                                        Steinberg::int16 keyCode,
                                        Steinberg::int16 modifiers) override { return Steinberg::kResultFalse; }
  Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect *size) override;
  Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect *newSize) override;
  Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) override { return Steinberg::kResultOk; }
  Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame *frame) override;
  Steinberg::tresult PLUGIN_API canResize() override;
  Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::ViewRect *rect) override;

  // Declaring this interface tells DPI-aware Windows hosts (Bitwig, Ableton,
  // etc.) that the plug-in renders in physical pixels and handles scaling
  // itself. Without it those hosts treat our ViewRect as logical DIP and
  // multiply it by the display scale, which makes the UI render at a fraction
  // of the window in the top-left corner. observe already reports and renders
  // in physical pixels and derives its own dpi scale from the OS, so we only
  // need to acknowledge the interface -- no size conversion is performed here.
  Steinberg::tresult PLUGIN_API setContentScaleFactor(ScaleFactor factor) override;

  OBJ_METHODS(OscillatorsVst3PlugView, FObject)
  DEFINE_INTERFACES
    DEF_INTERFACE(Steinberg::IPlugView)
    DEF_INTERFACE(Steinberg::IPlugViewContentScaleSupport)
  END_DEFINE_INTERFACES(FObject)
  REFCOUNT_METHODS(FObject)

private:
  int pluginWidth() const;
  int pluginHeight() const;
  void setPluginDimensions(int width, int height);
  void requestResizeFromHost();

  OscillatorsVst3Plugin *controller_ = nullptr;
  Steinberg::IPlugFrame *plug_frame_ = nullptr;
  ScaleFactor content_scale_ = 1.0f;
  std::unique_ptr<observe::ApplicationWindow> app_;
  std::unique_ptr<oscillators_observe::SpectrumView> spectrum_;
};

class OscillatorsVst3Plugin : public Steinberg::Vst::SingleComponentEffect
{
public:
  OscillatorsVst3Plugin();
  ~OscillatorsVst3Plugin() override = default;

  static Steinberg::FUnknown *createInstance(void * /*context*/)
  {
    return static_cast<Steinberg::Vst::IAudioProcessor *>(new OscillatorsVst3Plugin());
  }

  oscillators_observe::ResonateModel &model() { return model_; }

  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown *context) override;
  Steinberg::tresult PLUGIN_API terminate() override;
  Steinberg::tresult PLUGIN_API setupProcessing(Steinberg::Vst::ProcessSetup &setup) override;
  Steinberg::tresult PLUGIN_API setBusArrangements(Steinberg::Vst::SpeakerArrangement *inputs,
                                                   Steinberg::int32 numIns,
                                                   Steinberg::Vst::SpeakerArrangement *outputs,
                                                   Steinberg::int32 numOuts) override;
  Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData &data) override;
  Steinberg::IPlugView *PLUGIN_API createView(Steinberg::FIDString name) override;

private:
  oscillators_observe::ResonateModel model_;
  std::vector<float> mono_buffer_;
};
