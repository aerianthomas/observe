#include "client_window_decoration.h"

namespace observe
{

  static Bounds iconBounds(Bounds bounds)
  {
    float height = bounds.height();
    float width = bounds.width();
    float icon_y = (height + 2.0f) / 3.0f;
    float icon_width = height - 2.0f * icon_y;
    return {(width - icon_width) / 2.0f, icon_y, icon_width, icon_width};
  }

  ClientWindowDecoration::ClientWindowDecoration() : close_button_(HitTestResult::CloseButton), maximize_button_(HitTestResult::MaximizeButton),
                                                     minimize_button_(HitTestResult::MinimizeButton)
  {
    static constexpr int kIconColor = 0xffffffff;

    addChild(&close_button_);
    close_button_.setColor(kCloseButtonColor);
    addChild(&maximize_button_);
    addChild(&minimize_button_);

    close_button_.onDraw() += [this](Canvas &canvas)
    {
      canvas.setColor(kIconColor);
      Bounds bounds = iconBounds(close_button_.localBounds());
      canvas.segment(bounds.x(), bounds.y(), bounds.right(), bounds.bottom(), 1.0f, true);
      canvas.segment(bounds.x(), bounds.bottom(), bounds.right(), bounds.y(), 1.0f, true);
    };

    maximize_button_.onDraw() += [this](Canvas &canvas)
    {
      canvas.setColor(kIconColor);
      Bounds bounds = iconBounds(close_button_.localBounds());
      canvas.roundedRectangleBorder(bounds.x(), bounds.y(), bounds.width(), bounds.height(), 2.0f, 1.0f);
    };

    minimize_button_.onDraw() += [this](Canvas &canvas)
    {
      canvas.setColor(kIconColor);
      Bounds bounds = iconBounds(close_button_.localBounds());
      canvas.rectangle(bounds.x(), bounds.yCenter() - 1.0f, bounds.width(), 1.0f);
    };
  }
}
