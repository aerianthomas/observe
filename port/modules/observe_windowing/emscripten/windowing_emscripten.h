#pragma once

#if OBSERVE_EMSCRIPTEN
#include "windowing.h"

namespace observe
{
  class WindowEmscripten : public Window
  {
  public:
    static WindowEmscripten *running_instance_;
    static WindowEmscripten *runningInstance() { return running_instance_; }

    WindowEmscripten(int width, int height);

    void *initWindow() const override { return (void *)"#canvas"; }
    void *nativeHandle() const override { return (void *)"#canvas"; }

    void runEventLoop() override;
    void windowContentsResized(int width, int height) override;
    void show() override {}
    void showMaximized() override;
    bool maximized() { return false; }
    void hide() override {}
    void close() override {}
    bool isShowing() const override { return true; }
    void setWindowTitle(const std::string &title) override;
    IPoint maxWindowDimensions() const override;
    int initialWidth() const { return initial_width_; }
    int initialHeight() const { return initial_height_; }
    bool mouseRelativeMode() const override { return false; }

    void handleWindowResize(int window_width, int window_height);
    void runLoopCallback();

  private:
    int initial_width_ = 0;
    int initial_height_ = 0;
    float display_scale_ = 1.0f;
    bool maximized_ = false;
    int mouse_x_ = 0;
    int mouse_y_ = 0;
    long long start_microseconds_ = 0;
  };
}

#endif
