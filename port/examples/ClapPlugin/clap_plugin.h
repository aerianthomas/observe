#pragma once

#include <clap/helpers/plugin.hh>
#include <observe/app.h>

using ClapPluginBase = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                             clap::helpers::CheckingLevel::Maximal>;

class ClapPlugin : public ClapPluginBase
{
public:
  static clap_plugin_descriptor descriptor;

  explicit ClapPlugin(const clap_host *host);
  ~ClapPlugin() override;

#ifdef __linux__
  bool implementsPosixFdSupport() const noexcept override { return true; }
  void onPosixFd(int fd, clap_posix_fd_flags_t flags) noexcept override;
#endif

protected:
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
  int pluginWidth() const
  {
    if (app_ == nullptr)
      return 0;

#if __APPLE__
    return app_->width();
#else
    return app_->nativeWidth();
#endif
  }

  int pluginHeight() const
  {
    if (app_ == nullptr)
      return 0;

#if __APPLE__
    return app_->height();
#else
    return app_->nativeHeight();
#endif
  }

  void setPluginDimensions(int width, int height)
  {
    if (app_ == nullptr)
      return;
#if __APPLE__
    app_->setWindowDimensions(width, height);
#else
    app_->setNativeWindowDimensions(width, height);
#endif
  }

  std::unique_ptr<observe::ApplicationWindow> app_;
};
