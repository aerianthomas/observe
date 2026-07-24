// This is the VST3 equivalent of examples/ClapPlugin/clap_plugin.h.
// It reuses the same observe::ApplicationWindow GUI, wired into VST3's
// IPlugView instead of CLAP's gui extension. The method-for-method mapping is:
//
//   CLAP (clap_plugin.h)          VST3 (this file)
//   ---------------------------   ---------------------------
//   guiIsApiSupported()        -> Vst3PlugView::isPlatformTypeSupported()
//   guiCreate()/guiSetParent() -> Vst3PlugView::attached()
//   guiDestroy()               -> Vst3PlugView::removed()
//   guiSetSize()               -> Vst3PlugView::onSize()
//   guiGetSize()                -> Vst3PlugView::getSize()
//   guiAdjustSize()             -> Vst3PlugView::checkSizeConstraint()
//   guiCanResize()               -> Vst3PlugView::canResize()
//   host_->guiRequestResize()   -> IPlugFrame::resizeView() (stored via setFrame())
//
// We use Steinberg::Vst::SingleComponentEffect, which combines the audio
// processor (IComponent/IAudioProcessor) and the edit controller
// (IEditController) into one class/one CID. That mirrors how the CLAP
// example is a single ClapPlugin class doing everything, and it's the
// simplest way to add a VST3 target next to a CLAP target. Like the CLAP
// example, this plugin does no real audio processing -- it only exists to
// show a observe window.
//
// IMPORTANT: regenerate kVst3PluginProcessorUID for your own plugin
// (e.g. `uuidgen` on macOS/Linux, or Visual Studio's "Create GUID" tool).
// Never ship two different plugins with the same CID.

#pragma once

#include <base/source/fobject.h>
#include <memory>
#include <pluginterfaces/gui/iplugviewcontentscalesupport.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <public.sdk/source/vst/vstsinglecomponenteffect.h>
#include <observe/app.h>

// dev.observe.example vst3 plugin processor/controller class id (combined,
// since we use SingleComponentEffect). Regenerate this for your own plugin.
static const Steinberg::FUID kVst3PluginProcessorUID(0xB90B6D07, 0x1F9E4F1E, 0x8C2E9C1A, 0x2F6E9A47);

class Vst3Plugin;

//------------------------------------------------------------------------
// The GUI view. This owns the observe::ApplicationWindow, exactly like
// ClapPlugin owns app_ in the CLAP example.
class Vst3PlugView : public Steinberg::FObject,
                     public Steinberg::IPlugView,
                     public Steinberg::IPlugViewContentScaleSupport {
public:
  explicit Vst3PlugView(Vst3Plugin* controller);
  ~Vst3PlugView() override;

  // --- Steinberg::IPlugView ---
  Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) override;
  Steinberg::tresult PLUGIN_API attached(void* parent, Steinberg::FIDString type) override;
  Steinberg::tresult PLUGIN_API removed() override;
  Steinberg::tresult PLUGIN_API onWheel(float distance) override { return Steinberg::kResultFalse; }
  Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode,
                                          Steinberg::int16 modifiers) override {
    return Steinberg::kResultFalse;
  }
  Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode,
                                        Steinberg::int16 modifiers) override {
    return Steinberg::kResultFalse;
  }
  Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect* size) override;
  Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect* newSize) override;
  Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) override {
    return Steinberg::kResultOk;
  }
  Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame* frame) override;
  Steinberg::tresult PLUGIN_API canResize() override;
  Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::ViewRect* rect) override;

  // --- Steinberg::IPlugViewContentScaleSupport ---
  // Declaring this interface tells DPI-aware Windows hosts that the plug-in
  // renders in physical pixels and handles scaling itself. observe already
  // reports and renders in physical pixels and derives its own dpi scale from
  // the OS, so we acknowledge the interface without applying the factor again.
  Steinberg::tresult PLUGIN_API setContentScaleFactor(ScaleFactor factor) override;

  OBJ_METHODS(Vst3PlugView, FObject)
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

  Vst3Plugin* controller_ = nullptr;
  Steinberg::IPlugFrame* plug_frame_ = nullptr;
  std::unique_ptr<observe::ApplicationWindow> app_;
};

//------------------------------------------------------------------------
// The plugin itself: audio processor + edit controller combined.
class Vst3Plugin : public Steinberg::Vst::SingleComponentEffect {
public:
  Vst3Plugin();
  ~Vst3Plugin() override = default;

  static Steinberg::FUnknown* createInstance(void* /*context*/) {
    return static_cast<Steinberg::Vst::IAudioProcessor*>(new Vst3Plugin());
  }

  // --- Steinberg::IPluginBase ---
  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
  Steinberg::tresult PLUGIN_API terminate() override;

  // --- Steinberg::Vst::IAudioProcessor ---
  Steinberg::tresult PLUGIN_API setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs,
                                                   Steinberg::int32 numIns,
                                                   Steinberg::Vst::SpeakerArrangement* outputs,
                                                   Steinberg::int32 numOuts) override;
  Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override;

  // --- Steinberg::Vst::IEditController ---
  Steinberg::IPlugView* PLUGIN_API createView(Steinberg::FIDString name) override;
};
