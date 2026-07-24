#pragma once

#include "events.h"
#include "frame.h"
#include "scroll_bar.h"
#include "modules/observe_graphics/font.h"

#include <climits>

namespace observe
{
  class PopupMenu;
#if defined(__APPLE__) && defined(__MACH__)
  void setNativeMenuBar(const PopupMenu &menu);
#elif defined(WIN32)
  void setNativeMenuBar(const PopupMenu &menu); // Just declare - implementation is in win32/menu_bar.cpp
#else
  inline void setNativeMenuBar(const PopupMenu &menu) {} // Empty for other platforms
#endif

  class PopupMenu
  {
  public:
    static constexpr int kNotSet = INT_MIN;

    PopupMenu() = default;
    PopupMenu(const String &name, int id = -1, std::vector<PopupMenu> options = {}, bool is_break = false) : name_(name), id_(id), is_break_(is_break), options_(std::move(options)) {}

    void show(Frame *source, Point position = {kNotSet, kNotSet});
    void setAsNativeMenuBar() { setNativeMenuBar(*this); }

    PopupMenu &addOption(int option_id, const String &option_name)
    {
      options_.emplace_back(option_name, option_id);
      return options_.back();
    }

    PopupMenu &select(bool selected)
    {
      selected_ = selected;
      return *this;
    }

    bool selected() const { return selected_; }

    PopupMenu &enable(bool enabled)
    {
      enabled_ = enabled;
      return *this;
    }

    PopupMenu &withNativeKeyboardShortcut(int modifiers, std::string character)
    {
      shortcut_modifiers_ = modifiers;
      shortcut_character_ = character;
      return *this;
    }

    const int nativeShortcutModifiers() const { return shortcut_modifiers_; }
    const std::string &nativeShortcutCharacter() const { return shortcut_character_; }

    bool enabled() const { return enabled_; }

    auto &onSelection() { return on_selection_; }
    auto &onCancel() { return on_cancel_; }

    const auto &onSelection() const { return on_selection_; }
    const auto &onCancel() const { return on_cancel_; }

    void addSubMenu(PopupMenu sub_menu) { options_.push_back(std::move(sub_menu)); }
    void addBreak() { options_.push_back({"", -1, {}, true}); }

    const std::vector<PopupMenu> &options() const { return options_; }
    int size() const { return options_.size(); }

    int id() const { return id_; }
    const String &name() const { return name_; }
    bool isBreak() const { return is_break_; }
    bool hasOptions() const { return !options_.empty(); }

  private:
    CallbackList<void(int)> on_selection_;
    CallbackList<void()> on_cancel_;
    String name_;
    int id_ = -1;
    bool is_break_ = false;
    bool selected_ = false;
    bool enabled_ = true;

    int shortcut_modifiers_ = 0;
    std::string shortcut_character_;
    std::vector<PopupMenu> options_;
  };

  class PopupList : public ScrollableFrame
  {
  public:
    class Listener
    {
    public:
      virtual ~Listener() = default;
      virtual void optionSelected(const PopupMenu &option, PopupList *list) = 0;
      virtual void subMenuSelected(const PopupMenu &option, int selected_y, PopupList *list) = 0;
      virtual void mouseMovedOnMenu(Point position, PopupList *list) = 0;
      virtual void mouseDraggedOnMenu(Point position, PopupList *list) = 0;
      virtual void mouseUpOutside(Point position, PopupList *list) = 0;
    };

    PopupList() = default;

    void setOptions(std::vector<PopupMenu> options) { options_ = std::move(options); }
    void setFont(const Font &font) { font_ = font.withDpiScale(dpiScale()); }

    float renderHeight() const;
    float renderWidth() const;

    int yForIndex(int index) const;
    int hoverY() { return yForIndex(hover_index_); }
    int hoverIndex() const { return hover_index_; }
    int numOptions() const { return options_.size(); }
    const PopupMenu &option(int index) const { return options_[index]; }
    void selectHoveredIndex();
    void setHoverFromPosition(Point position);
    void setNoHover() { hover_index_ = -1; }
    void selectFromPosition(Point position);

    void draw(Canvas &canvas) override;
    void resized() override;

    void enableMouseUp(bool enable) { enable_mouse_up_ = enable; }

    void mouseExit(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseMove(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    bool mouseWheel(const MouseEvent &e) override
    {
      bool result = ScrollableFrame::mouseWheel(e);
      if (!isVisible())
        return result;

      for (Listener *listener : listeners_)
        listener->mouseMovedOnMenu(e.relativeTo(this).position, this);
      return result;
    }
    void addListener(Listener *listener) { listeners_.push_back(listener); }
    void resetOpenMenu() { menu_open_index_ = -1; }
    void setOpenMenu(int index) { menu_open_index_ = index; }
    void setOpacity(float opacity)
    {
      opacity_ = opacity;
      redraw();
    }

  private:
    std::vector<Listener *> listeners_;
    std::vector<PopupMenu> options_;
    float opacity_ = 0.0f;
    int hover_index_ = -1;
    int menu_open_index_ = -1;
    bool enable_mouse_up_ = false;
    Font font_;
  };

  class PopupMenuFrame : public Frame,
                         public PopupList::Listener,
                         public EventTimer
  {
  public:
    static constexpr int kMaxSubMenus = 4;
    static constexpr int kWaitForSelection = 20;
    static constexpr int kPauseMs = 400;

    PopupMenuFrame(PopupMenu menu);
    ~PopupMenuFrame() override;

    void draw(Canvas &canvas) override;

    void show(std::unique_ptr<PopupMenuFrame> self, Frame *source, Point point = {});
    void setFont(const Font &font)
    {
      font_ = font;
      setListFonts(font);
    }
    void setListFonts(const Font &font)
    {
      for (auto &list : lists_)
        list.setFont(font);
    }

    void exit();
    void hierarchyChanged() override;
    void focusChanged(bool is_focused, bool was_clicked) override;
    void visibilityChanged() override { opacity_animation_.target(isVisible(), true); }
    void timerCallback() override;

    void optionSelected(const PopupMenu &option, PopupList *list) override;
    void subMenuSelected(const PopupMenu &option, int selection_y, PopupList *list) override;
    void mouseMovedOnMenu(Point position, PopupList *list) override { moveHover(position, list); }
    void mouseDraggedOnMenu(Point position, PopupList *list) override { moveHover(position, list); }
    void mouseUpOutside(Point position, PopupList *list) override;
    void moveHover(Point position, const PopupList *list);
    float opacity() const { return opacity_animation_.value(); }

  private:
    PopupMenu menu_;
    Frame *parent_ = nullptr;
    bool done_ = false;
    Animation<float> opacity_animation_;
    PopupList lists_[kMaxSubMenus];
    int hover_index_ = -1;
    Font font_;
    PopupList *hover_list_ = nullptr;

    OBSERVE_LEAK_CHECKER(PopupMenuFrame)
  };

  class ValueDisplay : public Frame
  {
  public:
    ValueDisplay() { setIgnoresMouseEvents(true, false); }

    void draw(Canvas &canvas) override;

    void showDisplay(const String &text, Bounds bounds, Font::Justification justification);
    void setFont(const Font &font) { font_ = font; }

  private:
    Font font_;
    String text_;

    OBSERVE_LEAK_CHECKER(ValueDisplay)
  };
}