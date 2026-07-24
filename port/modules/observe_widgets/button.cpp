#include "button.h"

#include "embedded/fonts.h"
#include "modules/observe_graphics/theme.h"

namespace observe
{
  OBSERVE_THEME_COLOR(ButtonShadow, 0x88000000);

  OBSERVE_THEME_COLOR(TextButtonBackgroundOff, 0xff2c3033);
  OBSERVE_THEME_COLOR(TextButtonBackgroundOffHover, 0xff3e4245);
  OBSERVE_THEME_COLOR(TextButtonBackgroundOn, 0xff2c3033);
  OBSERVE_THEME_COLOR(TextButtonBackgroundOnHover, 0xff3e4245);

  OBSERVE_THEME_COLOR(TextButtonTextOff, 0xff848789);
  OBSERVE_THEME_COLOR(TextButtonTextOffHover, 0xffaaacad);
  OBSERVE_THEME_COLOR(TextButtonTextOn, 0xffaa88ff);
  OBSERVE_THEME_COLOR(TextButtonTextOnHover, 0xffbb99ff);

  OBSERVE_THEME_IMPLEMENT_COLOR(ToggleButton, ToggleButtonDisabled, 0xff4c4f52);
  OBSERVE_THEME_IMPLEMENT_COLOR(ToggleButton, ToggleButtonOff, 0xff848789);
  OBSERVE_THEME_IMPLEMENT_COLOR(ToggleButton, ToggleButtonOffHover, 0xffaaacad);
  OBSERVE_THEME_IMPLEMENT_COLOR(ToggleButton, ToggleButtonOn, 0xffaa88ff);
  OBSERVE_THEME_IMPLEMENT_COLOR(ToggleButton, ToggleButtonOnHover, 0xffbb99ff);

  OBSERVE_THEME_IMPLEMENT_COLOR(UiButton, UiButtonBackground, 0xff4c4f52);
  OBSERVE_THEME_IMPLEMENT_COLOR(UiButton, UiButtonBackgroundHover, 0xff606265);
  OBSERVE_THEME_IMPLEMENT_COLOR(UiButton, UiButtonText, 0xffdddddd);
  OBSERVE_THEME_IMPLEMENT_COLOR(UiButton, UiButtonTextHover, 0xffffffff);

  OBSERVE_THEME_IMPLEMENT_COLOR(UiButton, UiActionButtonBackground, 0xff9977ee);
  OBSERVE_THEME_IMPLEMENT_COLOR(UiButton, UiActionButtonBackgroundHover, 0xffaa88ff);
  OBSERVE_THEME_IMPLEMENT_COLOR(UiButton, UiActionButtonText, 0xffdddddd);
  OBSERVE_THEME_IMPLEMENT_COLOR(UiButton, UiActionButtonTextHover, 0xffffffff);

  OBSERVE_THEME_VALUE(TextButtonRounding, 9.0f);
  OBSERVE_THEME_VALUE(UiButtonRounding, 9.0f);
  OBSERVE_THEME_VALUE(UiButtonHoverRoundingMult, 0.7f);

  void Button::draw(Canvas &canvas)
  {
    draw(canvas, active_ ? hover_amount_.update() : 0.0f);

    if (hover_amount_.isAnimating())
      redraw();
  }

  void Button::mouseEnter(const MouseEvent &e)
  {
    hover_amount_.target(true);
    if (set_pointer_cursor_ && active_)
      setCursorStyle(MouseCursor::Pointing);

    redraw();
  }

  void Button::mouseExit(const MouseEvent &e)
  {
    hover_amount_.target(false);
    if (set_pointer_cursor_)
      setCursorStyle(MouseCursor::Arrow);

    redraw();
  }

  void Button::mouseDown(const MouseEvent &e)
  {
    if (!active_)
      return;

    alt_clicked_ = e.isAltDown();
    hover_amount_.target(false);
    redraw();

    if (toggle_on_mouse_down_)
      notify(toggle());
  }

  void Button::mouseUp(const MouseEvent &e)
  {
    if (!active_)
      return;

    redraw();
    if (localBounds().contains(e.position))
    {
      hover_amount_.target(true, true);
      if (!toggle_on_mouse_down_)
        notify(toggle());
    }
  }

  UiButton::UiButton(const std::string &text) : text_(text, Font(10, fonts::Lato_Regular_ttf)) {}

  UiButton::UiButton(const std::string &text, const Font &font) : text_(text, font) {}

  void UiButton::drawBackground(Canvas &canvas, float hover_amount)
  {
    if (action_)
      canvas.setBlendedColor(UiActionButtonBackground, UiActionButtonBackgroundHover, hover_amount);
    else
      canvas.setBlendedColor(UiButtonBackground, UiButtonBackgroundHover, hover_amount);

    int w = width();
    int h = height();
    float rounding = canvas.value(UiButtonRounding);

    if (isActive() || !border_when_inactive_)
    {
      float mult = hover_amount * canvas.value(UiButtonHoverRoundingMult) + (1.0f - hover_amount);
      canvas.roundedRectangle(0, 0, w, h, rounding * mult);
    }
    else
      canvas.roundedRectangleBorder(0, 0, w, h, rounding, 2.0f);
  }

  void UiButton::draw(Canvas &canvas, float hover_amount)
  {
    drawBackground(canvas, hover_amount);

    if (border_when_inactive_ && !isActive())
    {
      if (action_)
        canvas.setColor(UiActionButtonBackground);
      else
        canvas.setColor(UiButtonBackground);
    }
    else if (action_)
      canvas.setBlendedColor(UiActionButtonText, UiActionButtonTextHover, hover_amount);
    else
      canvas.setBlendedColor(UiButtonText, UiButtonTextHover, hover_amount);
    canvas.text(&text_, 0, 0, width(), height());
  }

  void IconButton::draw(Canvas &canvas, float hover_amount)
  {
    shadow_.setFillBrush(canvas.color(ButtonShadow));

    icon_.setFillBrush(canvas.blendedColor(ToggleButton::ToggleButtonOff,
                                           ToggleButton::ToggleButtonOffHover, hover_amount));
  }

  bool ToggleButton::toggle()
  {
    toggled_ = !toggled_;

    if (undoable_)
    {
      auto change_action = std::make_unique<ButtonChangeAction>(this, toggled_);
      if (undoSetupFunction())
        change_action->setSetupFunction(undoSetupFunction());
      addUndoableAction(std::move(change_action));
    }
    toggleValueChanged();
    return toggled_;
  }

  void ToggleIconButton::draw(Canvas &canvas, float hover_amount)
  {
    shadow_.setFillBrush(canvas.color(ButtonShadow));
    if (toggled())
      icon_.setFillBrush(canvas.blendedColor(ToggleButtonOn, ToggleButtonOnHover, hover_amount));
    else
      icon_.setFillBrush(canvas.blendedColor(ToggleButtonOff, ToggleButtonOffHover, hover_amount));
  }

  void ToggleIconButton::resized()
  {
    ToggleButton::resized();
    icon_.setBounds(localBounds());
    shadow_.setBounds(localBounds());
    computeShadowRadius();
  }

  ToggleTextButton::ToggleTextButton(const std::string &name) : ToggleButton(name), text_(name, Font(10, fonts::Lato_Regular_ttf)) {}

  ToggleTextButton::ToggleTextButton(const std::string &name, const Font &font) : ToggleButton(name), text_(name, font) {}

  void ToggleTextButton::drawBackground(Canvas &canvas, float hover_amount)
  {
    if (toggled())
      canvas.setBlendedColor(TextButtonBackgroundOn, TextButtonBackgroundOnHover, hover_amount);
    else
      canvas.setBlendedColor(TextButtonBackgroundOff, TextButtonBackgroundOffHover, hover_amount);

    float rounding = canvas.value(TextButtonRounding);
    canvas.roundedRectangle(0, 0, width(), height(), rounding);
  }

  void ToggleTextButton::draw(Canvas &canvas, float hover_amount)
  {
    if (draw_background_)
      drawBackground(canvas, hover_amount);

    if (toggled())
      canvas.setBlendedColor(TextButtonTextOn, TextButtonTextOnHover, hover_amount);
    else
      canvas.setBlendedColor(TextButtonTextOff, TextButtonTextOffHover, hover_amount);
    canvas.text(&text_, 0, 0, width(), height());
  }
}
