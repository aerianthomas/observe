#pragma once

#include <clap/helpers/plugin.hh>
#include <observe/app.h>

#include <memory>
#include <vector>

#include "resonate_view.h"

using ClapPluginBase = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                             clap::helpers::CheckingLevel::Maximal>;

class OscillatorsClapPlugin : public ClapPluginBase
{
public:
  static clap_plugin_descriptor descriptor;

  explicit OscillatorsClapPlugin(const clap_host *host);
  ~OscillatorsClapPlugin() override;

#ifdef __linux__
  bool implementsPosixFdSupport() const noexcept override { return true; }
  void onPosixFd(int fd, clap_posix_fd_flags_t flags) noexcept override;
#endif

protected:
  bool activate(double sample_rate, uint32_t min_frame_count, uint32_t max_frame_count) noexcept override;
  clap_process_status process(const clap_process *process) noexcept override;

  bool implementsAudioPorts() const noexcept override { return true; }
  uint32_t audioPortsCount(bool is_input) const noexcept override;
  bool audioPortsInfo(uint32_t index, bool is_input, clap_audio_port_info *info) const noexcept override;

  bool implementsGui() const noexcept override { return true; }
  bool guiIsApiSupported(const char *api, bool is_floating) noexcept override;
  bool guiCreate(const char *api, bool is_floating) noexcept override;
  void guiDestroy() noexcept override;
  bool guiSetParent(const clap_window *window) noexcept override;
  bool guiSetScale(double scale) noexcept override { return false; }
  bool guiCanResize() const noexcept override { return true; }
  bool guiGetResizeHints(clap_gui_resize_hints_t *hints) noexcept override;
  bool guiAdjustSize(uint32_t *width, uint32_t *height) noexcept override;
  bool guiSetSize(uint32_t width, uint32_t height) noexcept override;
  bool guiGetSize(uint32_t *width, uint32_t *height) noexcept override;

private:
  int pluginWidth() const;
  int pluginHeight() const;
  void setPluginDimensions(int width, int height);

  oscillators_observe::ResonateModel model_;
  std::vector<float> mono_buffer_;
  std::unique_ptr<observe::ApplicationWindow> app_;
  std::unique_ptr<oscillators_observe::SpectrumView> spectrum_;
};
