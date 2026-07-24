#include "graph_line.h"

namespace observe
{
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, LineColor, 0xffaa88ff);
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, LineFillColor, 0x669f88ff);
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, LineFillColor2, 0x669f88ff);
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, LineDisabledColor, 0xff4c4f52);
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, LineDisabledFillColor, 0x22666666);
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, CenterPoint, 0xff1d2125);
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, GridColor, 0x22ffffff);
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, HoverColor, 0xffffffff);
  OBSERVE_THEME_IMPLEMENT_COLOR(GraphLine, DragColor, 0x55ffffff);

  OBSERVE_THEME_IMPLEMENT_VALUE(GraphLine, LineWidth, 2.0f);
  OBSERVE_THEME_VALUE(PositionBulbWidth, 4.0f);

  GraphLine::GraphLine(int num_points, bool loop) : data_(num_points), loop_(loop) {}

  GraphLine::~GraphLine() = default;

  float GraphLine::fillLocation() const
  {
    if (fill_center_ == kBottom)
      return 0.0f;
    if (fill_center_ == kTop)
      return 1.0f;
    if (fill_center_ == kCustom)
      return custom_fill_center_;
    return 0.5f;
  }

  void GraphLine::draw(Canvas &canvas)
  {
    if (canvas.totallyClamped())
      return;

    if (filled_)
      drawFill(canvas, active_ ? LineFillColor : LineDisabledFillColor);
    drawLine(canvas, active_ ? LineColor : LineDisabledColor);
  }

  void GraphLine::drawLine(Canvas &canvas, theme::ColorId color_id)
  {
    canvas.setColor(color_id);
    float line_width = line_width_.compute(canvas.dpiScale(), width(), height(),
                                           canvas.dpiScale() * paletteValue(LineWidth));
    canvas.graphLine(data_, 0.0f, 0.0f, width(), height(), Dimension::nativePixels(line_width));
  }

  void GraphLine::drawFill(Canvas &canvas, theme::ColorId color_id)
  {
    if (fill_alpha_mult_ != 1.0f)
      canvas.setColor(canvas.color(color_id).withMultipliedAlpha(fill_alpha_mult_));
    else
      canvas.setColor(canvas.color(color_id));

    canvas.graphFill(data_, 0.0f, 0.0f, width(), height(), fillLocation());
  }
}