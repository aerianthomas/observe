#pragma once

#include "modules/observe_ui/frame.h"

namespace observe
{
  class BarList : public Frame
  {
  public:
    OBSERVE_THEME_DEFINE_COLOR(BarColor);

    struct Bar
    {
      float left = 0.0f;
      float top = 0.0f;
      float right = 0.0f;
      float bottom = 0.0f;
    };

    explicit BarList(int num_bars);
    ~BarList() override = default;

    void draw(Canvas &canvas) override;

    void setY(int index, float y)
    {
      bars_[index].top = y;
      redraw();
    }

    void positionBar(int index, float x, float y, float width, float height)
    {
      bars_[index] = {x, y, x + width, y + height};
      redraw();
    }

    int numBars() const { return num_bars_; }

  private:
    int num_bars_ = 0;
    std::unique_ptr<Bar[]> bars_;

    OBSERVE_LEAK_CHECKER(BarList)
  };
}