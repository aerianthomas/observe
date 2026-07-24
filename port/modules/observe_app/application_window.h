#pragma once

#include "application_editor.h"

#include <modules/observe_windowing/windowing.h>

namespace observe
{

  class ApplicationWindow : public ApplicationEditor
  {
  public:
    ApplicationWindow();
    ~ApplicationWindow() override;

    const std::string &title() const { return title_; }
    void setTitle(std::string title);

    bool isAlwaysOnTop() const { return always_on_top_; }
    void setWindowOnTop(bool on_top);

    void setWindowDecoration(Window::Decoration decoration)
    {
      decoration_ = decoration;
      if (decoration_ == Window::Decoration::Client)
        addClientDecoration();
    }

    void setNativeWindowDimensions(int width, int height)
    {
      setNativeBounds(nativeX(), nativeY(), width, height);
      if (window_)
        window_->setNativeWindowSize(width, height);
    }

    void setWindowDimensions(const Dimension &width, const Dimension &height);
    void setWindowDimensions(const Dimension &x, const Dimension &y, const Dimension &width,
                             const Dimension &height);

    void show();
    void show(void *parent_window);
    void show(const Dimension &width, const Dimension &height, void *parent_window);
    void show(const Dimension &width, const Dimension &height);
    void show(const Dimension &x, const Dimension &y, const Dimension &width, const Dimension &height);
    void showMaximized();
    void hide();
    void close();
    bool isShowing() const;
    void runEventLoop();

  private:
    void showWindow(bool maximized);

    IPoint initial_position_;
    Window::Decoration decoration_ = Window::Decoration::Native;
    std::string title_;
    bool always_on_top_ = false;
    std::unique_ptr<Window> window_;
  };
}
