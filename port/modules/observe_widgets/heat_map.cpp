#include "heat_map.h"

namespace observe
{
  HeatMap::HeatMap() = default;
  HeatMap::HeatMap(int width, int height) : data_(width, height) {}

  HeatMap::~HeatMap() = default;

  void HeatMap::draw(Canvas &canvas)
  {
    if (canvas.totallyClamped() || data_.width() <= 0 || data_.height() <= 0)
      return;

    canvas.setColor(observe::Brush::horizontal(gradient_));
    canvas.heatMap(data_, 0.0f, 0.0f, width(), height());
  }
}