#pragma once

#include "modules/observe_graphics/animation.h"
#include "modules/observe_ui/frame.h"

namespace observe
{
  class ClientDecoratorButton : public Frame
  {
  public:
    static constexpr int kDefaultHoverColor = 0x88888888;

    ClientDecoratorButton(HitTestResult hit_test_result) : hit_test_result_(hit_test_result), color_(kDefaultHoverColor)
    {
      hover_animation_.setSourceValue(0.0f);
      hover_animation_.setTargetValue(1.0f);
    }

    void mouseEnter(const MouseEvent &e) override
    {
      hover_animation_.target(true);
      redraw();
    }

    void mouseExit(const MouseEvent &e) override
    {
      hover_animation_.target(false);
      redraw();
    }

    void draw(Canvas &canvas) override
    {
      canvas.setColor(color_.withAlpha(color_.alpha() * hover_animation_.update()));
      canvas.fill(0, 0, width(), height());

      if (hover_animation_.isAnimating())
        redraw();
    }

    void setColor(const Color &color) { color_ = color; }

    HitTestResult hitTest(const Point &position) const override { return hit_test_result_; }

  private:
    Animation<float> hover_animation_;
    HitTestResult hit_test_result_ = HitTestResult::Client;
    Color color_;
  };

  class ClientWindowDecoration : public Frame
  {
  public:
    static constexpr int kButtonWidth = 46;
    static constexpr int kButtonHeight = 28;
    static constexpr int kCloseButtonColor = 0xffc42b1c;

    static int requiredWidth() { return 3 * kButtonWidth; }
    static int requiredHeight() { return kButtonHeight; }

    ClientWindowDecoration();

    void resized() override
    {
      close_button_.setBounds(width() - kButtonWidth, 0, kButtonWidth, height());
      maximize_button_.setBounds(close_button_.x() - kButtonWidth, 0, kButtonWidth, height());
      minimize_button_.setBounds(maximize_button_.x() - kButtonWidth, 0, kButtonWidth, height());
    }

  private:
    ClientDecoratorButton close_button_;
    ClientDecoratorButton maximize_button_;
    ClientDecoratorButton minimize_button_;

    OBSERVE_LEAK_CHECKER(ClientWindowDecoration)
  };
}
