#pragma once

// Internal Win32 native menu bar plumbing shared between observe_ui and
// observe_windowing. These functions are NOT part of the public observe API; the
// public entry point is observe::setNativeMenuBar in popup_menu.h.
//
// HWND is passed as void* so this header stays free of <windows.h> and can be
// included from translation units that have not (yet) pulled it in.

namespace observe
{
  // Registers a window as the active menu-bar host and applies the current menu
  // to it. Call once per top-level window after it is created.
  void attachNativeMenuBar(void *hwnd);

  // Unregisters a window. Call when the window is destroyed so subsequent
  // menu updates are not applied to a dead handle.
  void detachNativeMenuBar(void *hwnd);

  // Dispatches a WM_COMMAND wParam to the matching menu item's callbacks.
  // Returns true if the command matched an enabled menu item.
  bool handleMenuCommand(unsigned long long w_param);
}
