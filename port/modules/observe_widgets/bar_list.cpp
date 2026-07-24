#include "bar_list.h"

#include "modules/observe_graphics/theme.h"

namespace observe
{
  OBSERVE_THEME_IMPLEMENT_COLOR(BarList, BarColor, 0xffaa88ff);

  BarList::BarList(int num_bars) : num_bars_(num_bars), bars_(std::make_unique<Bar[]>(num_bars)) {}

  void BarList::draw(Canvas &canvas)
  {
    canvas.setColor(BarColor);

    for (int i = 0; i < num_bars_; ++i)
    {
      const Bar &bar = bars_[i];
      canvas.rectangle(bar.left, bar.top, bar.right - bar.left, bar.bottom - bar.top);
    }
  }
}