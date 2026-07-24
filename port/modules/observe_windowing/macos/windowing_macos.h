#pragma once

#if OBSERVE_MAC
#include "windowing.h"

#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>

namespace observe
{
  class WindowMac;
}

@interface ObserveDraggingSource : NSObject <NSDraggingSource>
@end

@interface ObserveAppViewDelegate : NSObject <MTKViewDelegate>
@property(nonatomic) observe::WindowMac *observe_window;
@property long long start_microseconds;
@end

@interface ObserveAppView : MTKView <NSDraggingDestination>
@property(nonatomic) observe::WindowMac *observe_window;
@property(strong) ObserveDraggingSource *drag_source;
@property bool allow_quit;
@property NSPoint mouse_down_screen_position;

- (instancetype)initWithFrame:(NSRect)frame_rect inWindow:(observe::WindowMac *)window;
@end

@interface ObserveAppWindowDelegate : NSObject <NSWindowDelegate>
@property(nonatomic) observe::WindowMac *observe_window;
@property(nonatomic, retain) NSWindow *window_handle;
@property bool resizing_horizontal;
@property bool resizing_vertical;
@end

@interface ObserveAppDelegate : NSObject <NSApplicationDelegate>
@property(nonatomic, retain) NSWindow *window_handle;
@property(nonatomic, strong) ObserveAppWindowDelegate *window_delegate;
@property observe::WindowMac *observe_window;
@end

namespace observe
{
  class WindowMac : public Window
  {
  public:
    WindowMac(int x, int y, int width, int height, float scale, Decoration decoration);
    WindowMac(int width, int height, float scale, void *parent_handle);

    ~WindowMac() override;

    void createWindow();
    void closeWindow();
    void setParentWindow(NSWindow *window);
    void resetBackingScale();

    void *nativeHandle() const override { return (__bridge void *)view_; }
    void *initWindow() const override;

    void runEventLoop() override;
    void windowContentsResized(int width, int height) override;
    void show() override;
    void showMaximized() override;
    void hide() final;
    void close() final;
    bool isShowing() const override;

    void setWindowTitle(const std::string &title) override;
    IPoint maxWindowDimensions() const override;
    void setAlwaysOnTop(bool on_top) override;

    void handleNativeResize(int width, int height);
    bool isPopup() const { return decoration_ == Decoration::Popup; }

  private:
    static bool running_event_loop_;
    NSWindow *window_handle_ = nullptr;
    NSView *parent_view_ = nullptr;
    ObserveAppView *view_ = nullptr;
    ObserveAppViewDelegate *view_delegate_ = nullptr;
    NSRect last_content_rect_{};
    Decoration decoration_ = Decoration::Native;
  };
}

#endif
