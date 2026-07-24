#pragma once

#include "modules/observe_graphics/theme.h"
#include "modules/observe_ui/frame.h"

namespace observe
{
  class HeatMap : public Frame
  {
  public:
    HeatMap();
    HeatMap(int width, int height);
    ~HeatMap() override;

    void setDimensions(int width, int height)
    {
      data_.setDimensions(width, height);
      redraw();
    }

    void setOctaves(float octaves)
    {
      data_.setOctaves(octaves);
      redraw();
    }

    void draw(Canvas &canvas) override;

    void setGradient(observe::Gradient gradient)
    {
      gradient_ = std::move(gradient);
      redraw();
    }

    float at(int x, int y) const { return data_.at(x, y); }
    void set(int x, int y, float val)
    {
      data_.set(x, y, val);
      redraw();
    }

    int dataWidth() const { return data_.width(); }
    int dataHeight() const { return data_.height(); }

  private:
    HeatMapData data_;
    observe::Gradient gradient_ = observe::Gradient::kMagma;

    OBSERVE_LEAK_CHECKER(HeatMap)
  };
}