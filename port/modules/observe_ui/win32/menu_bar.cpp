#if OBSERVE_WINDOWS

#include "popup_menu.h"
#include "win32/menu_bar.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>

#include <map>
#include <mutex>

namespace observe
{
  // Owns the single active native menu bar. The model mirrors macOS: there is
  // one logical menu bar, hosted by at most one top-level window at a time
  // (the most recently attached one). Updating the menu re-applies it live to
  // that window.
  class NativeMenu
  {
  public:
    static NativeMenu &instance()
    {
      static NativeMenu native_menu;
      return native_menu;
    }

    // Stores the menu and, if a host window is attached, applies it live.
    void setMenu(const PopupMenu &menu)
    {
      menu_ = menu;
      apply();
    }

    // Registers the host window and applies the current menu to it.
    void attach(HWND hwnd)
    {
      hwnd_ = hwnd;
      apply();
    }

    // Releases the host window. The window destroys its own attached HMENU, so
    // we only drop our references here.
    void detach(HWND hwnd)
    {
      if (hwnd_ != hwnd)
        return;
      hwnd_ = nullptr;
      menu_handle_ = nullptr;
      clearItems();
    }

    bool handleCommand(WPARAM w_param)
    {
      int command_id = LOWORD(w_param);
      const PopupMenu *item = findItem(command_id);
      if (item && item->enabled())
      {
        item->onSelection().callback(item->id());
        menu_.onSelection().callback(item->id());
        return true;
      }
      return false;
    }

  private:
    // Rebuilds the native menu from the stored PopupMenu and applies it to the
    // host window, replacing and freeing any previously applied menu.
    void apply()
    {
      if (hwnd_ == nullptr || !IsWindow(hwnd_))
        return;

      clearItems();
      HMENU new_menu = buildMenu(menu_);
      SetMenu(hwnd_, new_menu);
      if (menu_handle_)
        DestroyMenu(menu_handle_);
      menu_handle_ = new_menu;
      DrawMenuBar(hwnd_);
    }

    HMENU buildMenu(const PopupMenu &popup_menu)
    {
      HMENU menu = CreateMenu();

      for (const auto &item : popup_menu.options())
      {
        std::wstring name = item.name().toWide();

        if (item.isBreak())
        {
          AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        }
        else if (!item.options().empty())
        {
          HMENU submenu = buildMenu(item);
          AppendMenuW(menu, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(submenu), name.c_str());
        }
        else
        {
          int command_id = nextId();
          registerItem(command_id, &item);

          UINT flags = MF_STRING;
          if (!item.enabled())
            flags |= MF_GRAYED;
          if (item.selected())
            flags |= MF_CHECKED;

          AppendMenuW(menu, flags, static_cast<UINT_PTR>(command_id), name.c_str());
        }
      }
      return menu;
    }

    int nextId() { return ++next_id_; }

    void registerItem(int command_id, const PopupMenu *item)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      item_lookup_[command_id] = item;
    }

    const PopupMenu *findItem(int command_id)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      auto it = item_lookup_.find(command_id);
      return it != item_lookup_.end() ? it->second : nullptr;
    }

    // Resets the command-id -> item mapping so it does not grow without bound
    // each time the menu is rebuilt.
    void clearItems()
    {
      std::lock_guard<std::mutex> lock(mutex_);
      item_lookup_.clear();
      next_id_ = 0;
    }

    PopupMenu menu_;
    // Map from Win32 menu command ID to PopupMenu pointer for callback dispatch.
    std::map<int, const PopupMenu *> item_lookup_;
    int next_id_ = 0;
    HWND hwnd_ = nullptr;
    HMENU menu_handle_ = nullptr;
    std::mutex mutex_;
  };

  void setNativeMenuBar(const PopupMenu &popup_menu)
  {
    NativeMenu::instance().setMenu(popup_menu);
  }

  void attachNativeMenuBar(void *hwnd)
  {
    NativeMenu::instance().attach(static_cast<HWND>(hwnd));
  }

  void detachNativeMenuBar(void *hwnd)
  {
    NativeMenu::instance().detach(static_cast<HWND>(hwnd));
  }

  bool handleMenuCommand(unsigned long long w_param)
  {
    return NativeMenu::instance().handleCommand(static_cast<WPARAM>(w_param));
  }
}

#endif
