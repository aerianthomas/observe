#include "scroll_bar.h"

#include "modules/observe_graphics/theme.h"

namespace observe
{
  OBSERVE_THEME_COLOR(ScrollBarDefault, 0x22ffffff);
  OBSERVE_THEME_COLOR(ScrollBarDown, 0x55ffffff);

  OBSERVE_THEME_VALUE(ScrollBarWidth, 20.0f);

  void ScrollBar::draw(Canvas &canvas)
  {
    if (!active_ || range_ <= 0)
      return;

    float y_ratio = position_ / range_;
    float height_ratio = view_height_ * 1.0f / range_;
    int h = height();

    canvas.setBlendedColor(ScrollBarDefault, ScrollBarDown, color_.update());
    float w = width_.update();

    float rounding = std::min(width_.sourceValue() / 2.0f, rounding_);
    float x = left_ ? 0.0f : width() - w;
    canvas.roundedRectangle(x, y_ratio * h, w, height_ratio * h, rounding);

    if (width_.isAnimating() || color_.isAnimating())
      redraw();
  }

  void ScrollBar::mouseEnter(const MouseEvent &e)
  {
    width_.target(true);
    redraw();
  }

  void ScrollBar::mouseExit(const MouseEvent &e)
  {
    width_.target(false);
    redraw();
  }

  void ScrollBar::mouseDown(const MouseEvent &e)
  {
    redraw();
    color_.target(true);

    int max_value = range_ - view_height_;
    if (!active_ || max_value <= 0 || range_ <= 0)
      return;

    last_drag_ = e.position.y;
  }

  void ScrollBar::mouseUp(const MouseEvent &e)
  {
    color_.target(false);
    redraw();
  }

  void ScrollBar::mouseDrag(const MouseEvent &e)
  {
    float delta = range_ * (e.position.y - last_drag_) / height();
    last_drag_ = e.position.y;

    position_ += delta;
    position_ = std::min<float>(range_ - view_height_, std::max(0.0f, position_));

    for (auto &callback : callbacks_)
      callback(std::round(position_));
    redraw();
  }

  float ScrollableFrame::bottomPadding()
  {
    if (container_.hasLayout())
      return container_.layout().paddingBottom().compute(dpiScale(), width(), height()) / dpiScale();
    return 0;
  }

  std::pair<const Frame *, float> ScrollableFrame::findBottomMostChild()
  {
    Frame *bottom_most = nullptr;
    float bottom = 0;
    for (const auto &child : container_.children())
    {
      float margin_bottom = 0.0f;
      if (child->hasLayout())
        margin_bottom = child->layout().marginBottom().compute(dpiScale(), width(), height()) / dpiScale();
      if (child->bottom() + margin_bottom > bottom)
      {
        bottom = child->bottom() + margin_bottom;
        bottom_most = child;
      }
    }

    return {bottom_most, bottom};
  }

  void ScrollableFrame::updateScrollableHeight(const Frame *changed)
  {
    float padding = bottomPadding();
    float changed_bottom = changed->bottom() + padding;
    if (changed_bottom > container_.height() + 1.0f)
    {
      bottom_most_child_ = changed;
      setScrollableHeight(changed_bottom);
    }
    else if (changed_bottom < container_.height() - 1.0f && changed == bottom_most_child_)
      updateScrollableHeight();
  }

  void ScrollableFrame::updateScrollableHeight()
  {
    auto bottom_most = findBottomMostChild();
    bottom_most_child_ = bottom_most.first;
    setScrollableHeight(bottom_most.second + bottomPadding());
  }

  void ScrollableFrame::resized()
  {
    int scroll_bar_width = paletteValue(ScrollBarWidth);
    int x = scroll_bar_left_ ? 0 : width() - scroll_bar_width;
    smooth_position_ = y_position_;
    scroll_bar_.setBounds(x, 0, scroll_bar_width, height());

    if (container_.hasLayout())
    {
      IBounds bounding_box = container_.computeLayoutBoundingBox({0, 0, nativeWidth(), nativeHeight()});
      setScrollableHeight(bounding_box.height() / dpiScale());
      bottom_most_child_ = findBottomMostChild().first;
    }
    else
    {
      auto bottom_most = findBottomMostChild();
      bottom_most_child_ = bottom_most.first;
      setScrollableHeight(bottom_most.second + bottomPadding());
    }
  }
}