#include <observe/graphics/rect.h>

namespace observe
{

    Rect::Rect(
        float x,
        float y,
        float width,
        float height)
        : x(x),
          y(y),
          width(width),
          height(height)
    {
    }

    Vec2 Rect::center() const noexcept
    {
        return Vec2(
            x + width * 0.5f,
            y + height * 0.5f);
    }

    Vec2 Rect::halfSize() const noexcept
    {
        return Vec2(
            width * 0.5f,
            height * 0.5f);
    }

}
