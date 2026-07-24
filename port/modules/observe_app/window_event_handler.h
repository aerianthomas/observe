#pragma once

#include "application_editor.h"
#include "modules/observe_ui/events.h"
#include "modules/observe_windowing/windowing.h"

namespace observe
{
  class Frame;

  class WindowEventHandler : public Window::EventHandler
  {
  public:
    WindowEventHandler() = delete;
    WindowEventHandler(const WindowEventHandler &) = delete;

    WindowEventHandler(ApplicationEditor *editor, Frame *frame);
    ~WindowEventHandler() override;

    Frame *contentFrame() const { return content_frame_; }
    void setKeyboardFocus(Frame *frame);
    void giveUpFocus(Frame *frame);

    Point lastMousePosition() const { return last_mouse_position_; }
    IPoint convertToNative(const Point &point) const { return window_->convertToNative(point); }
    Point convertToLogical(const IPoint &point) const { return window_->convertToLogical(point); }

    MouseEvent mouseEvent(int x, int y, int button_state, int modifiers);
    MouseEvent buttonMouseEvent(MouseButton button_id, int x, int y, int button_state, int modifiers);

    HitTestResult handleHitTest(int x, int y) override;
    HitTestResult currentHitTest() const override { return current_hit_test_; }
    void handleMouseMove(int x, int y, int button_state, int modifiers) override;
    void handleMouseDown(MouseButton button_id, int x, int y, int button_state, int modifiers,
                         int repeat) override;
    void handleMouseUp(MouseButton button_id, int x, int y, int button_state, int modifiers,
                       int repeat) override;
    void handleMouseEnter(int x, int y) override;
    void handleMouseLeave(int x, int y, int button_state, int modifiers) override;
    void handleMouseWheel(float delta_x, float delta_y, float precise_x, float precise_y, int x,
                          int y, int button_state, int modifiers, bool momentum) override;

    bool handleKeyDown(const KeyEvent &e);
    bool handleKeyUp(const KeyEvent &e);
    bool handleKeyDown(KeyCode key_code, int modifiers, bool repeat) override;
    bool handleKeyUp(KeyCode key_code, int modifiers) override;

    bool handleTextInput(const std::string &text) override;
    bool hasActiveTextEntry() override;

    void handleFocusLost() override;
    void handleFocusGained() override;
    void handleResized(int width, int height) override;
    void handleAdjustResize(int *width, int *height, bool horizontal_resize, bool vertical_resize) override;

    void handleWindowShown() override;
    void handleWindowHidden() override;
    bool handleCloseRequested() override;

    bool handleFileDrag(int x, int y, const std::vector<std::string> &files) override;
    void handleFileDragLeave() override;
    bool handleFileDrop(int x, int y, const std::vector<std::string> &files) override;

    bool isDragDropSource() override;
    std::string startDragDropSource() override;
    void cleanupDragDropSource() override;

  private:
    Frame *dragDropFrame(Point point, const std::vector<std::string> &files) const;

    ApplicationEditor *editor_ = nullptr;
    Window *window_ = nullptr;
    Frame *content_frame_ = nullptr;
    Frame *mouse_hovered_frame_ = nullptr;
    Frame *temporary_frame_ = nullptr;
    Frame *mouse_down_frame_ = nullptr;
    Frame *keyboard_focused_frame_ = nullptr;
    Frame *drag_drop_target_frame_ = nullptr;

    Point last_mouse_position_ = {0, 0};
    HitTestResult current_hit_test_ = HitTestResult::Client;

    OBSERVE_LEAK_CHECKER(WindowEventHandler)
  };
}
