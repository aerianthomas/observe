#pragma once

#include "color.h"
#include "theme.h"

#include <map>
#include <vector>

namespace observe
{
  class Palette
  {
  public:
    static constexpr int kInvalidId = -2;
    static constexpr unsigned int kInvalidColor = 0xffff00ff;
    static constexpr float kNotSetValue = -99999.0f;
    static constexpr int kNotSetId = -1;
    static constexpr char kEncodingSeparator = '@';

    Palette() = default;

    int numColors() const { return colors_.size(); }

    Brush colorIndex(int index) const
    {
      OBSERVE_ASSERT(index >= 0 && index < colors_.size());
      return colors_[index];
    }

    const std::vector<Brush> &colorList() const { return colors_; }

    void initWithDefaults();
    void sortColors();

    std::map<std::string, std::vector<theme::ColorId>> colorIdList(theme::OverrideId override_id);
    std::map<std::string, std::vector<theme::ValueId>> valueIdList(theme::OverrideId override_id);

    void setEditColor(int index, const Brush &color)
    {
      OBSERVE_ASSERT(index >= 0 && index < colors_.size());
      colors_[index] = color;
    }

    void setColorIndexFrom(int index, const Color &color)
    {
      OBSERVE_ASSERT(index >= 0 && index < colors_.size());
      colors_[index].gradient().setColor(0, color);
    }

    void setColorIndexTo(int index, const Color &color)
    {
      OBSERVE_ASSERT(index >= 0 && index < colors_.size());
      colors_[index].gradient().setColor(1, color);
    }

    void toggleColorIndexStyle(int index)
    {
      OBSERVE_ASSERT(index >= 0 && index < colors_.size());
      if (colors_[index].position().shape == GradientPosition::InterpolationShape::Solid)
      {
        colors_[index].gradient().setResolution(2);
        colors_[index].position().shape = GradientPosition::InterpolationShape::Vertical;
      }
      else if (colors_[index].position().shape == GradientPosition::InterpolationShape::Vertical)
      {
        colors_[index].gradient().setResolution(2);
        colors_[index].position().shape = GradientPosition::InterpolationShape::Horizontal;
      }
      else
      {
        colors_[index].gradient().setResolution(1);
        colors_[index].position().shape = GradientPosition::InterpolationShape::Solid;
      }
    }

    bool color(theme::OverrideId override_id, theme::ColorId color_id, Brush &color)
    {
      color_map_[override_id].try_emplace(color_id, kNotSetId);

      if (color_map_[override_id][color_id] == kNotSetId)
        return false;

      if (color_map_[override_id][color_id] == kInvalidId)
        color = Brush::solid(kInvalidColor);
      else
        color = colors_[color_map_[override_id][color_id]];
      return true;
    }

    void setColorMap(theme::OverrideId override_id, theme::ColorId color_id, int index)
    {
      color_map_[override_id][color_id] = index;
    }

    void setColor(theme::OverrideId override_id, theme::ColorId color_id, const Color &color)
    {
      int index = addColor(color);
      color_map_[override_id][color_id] = index;
    }

    void setColor(theme::OverrideId override_id, theme::ColorId color_id, const Brush &color)
    {
      int index = addBrush(color);
      color_map_[override_id][color_id] = index;
    }

    void setColor(theme::ColorId color_id, const Color &color) { setColor({}, color_id, color); }
    void setColor(theme::ColorId color_id, const Brush &color) { setColor({}, color_id, color); }

    void setValue(theme::OverrideId override_id, theme::ValueId value_id, float value)
    {
      value_map_[override_id][value_id] = value;
    }

    void setValue(theme::ValueId value_id, float value) { setValue({}, value_id, value); }

    void removeValue(theme::OverrideId override_id, theme::ValueId value_id)
    {
      if (value_map_[override_id].count(value_id))
        value_map_[override_id].erase(value_id);
    }

    void removeValue(theme::ValueId value_id) { removeValue({}, value_id); }

    int colorMap(theme::OverrideId override_id, theme::ColorId color_id)
    {
      return color_map_[override_id][color_id];
    }

    bool value(theme::OverrideId override_id, theme::ValueId value_id, float &result)
    {
      value_map_[override_id].try_emplace(value_id, kNotSetValue);
      result = value_map_[override_id][value_id];
      return result != kNotSetValue;
    }

    int addColor(const Color &color = 0xffff00ff)
    {
      colors_.emplace_back(Brush::solid(color));
      return colors_.size() - 1;
    }

    int addBrush(const Brush &color)
    {
      colors_.emplace_back(color);
      return colors_.size() - 1;
    }

    void clear()
    {
      color_map_.clear();
      value_map_.clear();
      colors_.clear();
    }

    void removeColor(int index);

    std::string encode() const;
    void decode(const std::string &data);

  private:
    std::vector<Brush> colors_;
    std::map<theme::OverrideId, std::map<theme::ColorId, int>> color_map_;
    std::map<theme::OverrideId, std::map<theme::ValueId, float>> value_map_;
  };
}