#pragma once

#include "modules/observe_graphics/theme.h"
#include "modules/observe_ui/frame.h"

namespace observe
{
  class GraphLine : public Frame
  {
  public:
    OBSERVE_THEME_DEFINE_COLOR(LineColor);
    OBSERVE_THEME_DEFINE_COLOR(LineFillColor);
    OBSERVE_THEME_DEFINE_COLOR(LineFillColor2);
    OBSERVE_THEME_DEFINE_COLOR(LineDisabledColor);
    OBSERVE_THEME_DEFINE_COLOR(LineDisabledFillColor);
    OBSERVE_THEME_DEFINE_COLOR(CenterPoint);
    OBSERVE_THEME_DEFINE_COLOR(GridColor);
    OBSERVE_THEME_DEFINE_COLOR(HoverColor);
    OBSERVE_THEME_DEFINE_COLOR(DragColor);

    OBSERVE_THEME_DEFINE_VALUE(LineWidth);

    enum FillCenter
    {
      kCenter,
      kBottom,
      kTop,
      kCustom
    };

    explicit GraphLine(int num_points, bool loop = false);
    ~GraphLine() override;

    void draw(Canvas &canvas) override;

    float at(int index) const { return data_[index]; }
    void set(int index, float val)
    {
      OBSERVE_ASSERT(index < data_.numPoints() && index >= 0);
      data_[index] = val;
      redraw();
    }

    bool isFilled() const { return filled_; }
    void setFilled(bool fill) { filled_ = fill; }
    void setFillCenter(FillCenter fill_center) { fill_center_ = fill_center; }
    void setFillCenter(float center)
    {
      custom_fill_center_ = center;
      fill_center_ = kCustom;
      redraw();
    }
    float fillLocation() const;

    int numPoints() const { return data_.numPoints(); }

    bool active() const { return active_; }
    void setActive(bool active) { active_ = active; }
    void setFillAlphaMult(float mult) { fill_alpha_mult_ = mult; }

  private:
    void drawLine(Canvas &canvas, theme::ColorId color_id);
    void drawFill(Canvas &canvas, theme::ColorId color_id);

    GraphData data_;
    Dimension line_width_;

    bool filled_ = false;
    FillCenter fill_center_ = kCenter;
    float custom_fill_center_ = 0.0f;
    float fill_alpha_mult_ = 1.0f;

    bool active_ = true;
    bool loop_ = false;

    OBSERVE_LEAK_CHECKER(GraphLine)
  };
}