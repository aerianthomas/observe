#if OBSERVE_MAC
#include "popup_menu.h"

#include <Cocoa/Cocoa.h>

namespace observe {
  class NativeMenu {
  public:
    static NativeMenu& instance() {
      static NativeMenu native_menu;
      return native_menu;
    }

    static const PopupMenu& menu() { return instance().menu_; }
    static void setPopup(const PopupMenu& menu) { instance().menu_ = menu; }

  private:
    PopupMenu menu_;
  };
}

@interface PopupMenuTarget : NSObject
- (void)invoke:(id)sender;
@end

@implementation PopupMenuTarget {
}

- (void)invoke:(id)sender {
  const observe::PopupMenu* menu = (const observe::PopupMenu*)[[sender representedObject] pointerValue];
  menu->onSelection().callback(menu->id());
  observe::NativeMenu::menu().onSelection().callback(menu->id());
}
@end

namespace observe {
  NSMenu* buildMenu(const PopupMenu& popup_menu, id target) {
    NSMenu* menu = [[NSMenu alloc] init];
    auto handler = [[PopupMenuTarget alloc] init];

    for (const auto& item : popup_menu.options()) {
      NSString* name = [NSString stringWithUTF8String:item.name().toUtf8().c_str()];

      if (item.isBreak())
        [menu addItem:[NSMenuItem separatorItem]];
      else if (!item.options().empty()) {
        NSMenuItem* submenu_item = [[NSMenuItem alloc] initWithTitle:name
                                                              action:nil
                                                       keyEquivalent:@""];

        [submenu_item setSubmenu:buildMenu(item, target)];
        [menu addItem:submenu_item];
      }
      else {
        NSString* key = [NSString stringWithUTF8String:item.nativeShortcutCharacter().c_str()];
        NSMenuItem* menu_item = [[NSMenuItem alloc] initWithTitle:name
                                                           action:@selector(invoke:)
                                                    keyEquivalent:key];
        int modifiers = item.nativeShortcutModifiers();
        if (modifiers) {
          NSEventModifierFlags flags = 0;
          if (modifiers & Modifiers::kModifierCmd)
            flags = flags | NSEventModifierFlagCommand;
          if (modifiers & Modifiers::kModifierOption)
            flags = flags | NSEventModifierFlagOption;
          if (modifiers & Modifiers::kModifierMacCtrl)
            flags = flags | NSEventModifierFlagControl;
          if (modifiers & Modifiers::kModifierShift)
            flags = flags | NSEventModifierFlagShift;

          [menu_item setKeyEquivalentModifierMask:flags];
        }
        if (item.enabled()) {
          [menu_item setTarget:handler];
          [menu_item setRepresentedObject:[NSValue valueWithPointer:&item]];
        }
        if (item.selected())
          [menu_item setState:NSControlStateValueOn];

        [menu addItem:menu_item];
      }
    }
    return menu;
  }

  void setNativeMenuBar(const PopupMenu& popup_menu) {
    @autoreleasepool {
      NativeMenu::setPopup(popup_menu);
      [NSApp setMainMenu:buildMenu(NativeMenu::menu(), nil)];
    }
  }
}

#endif
