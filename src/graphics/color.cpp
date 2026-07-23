#include <observe/graphics/color.h>

#include <algorithm>

namespace observe
{

    Color::Color(
        float r,
        float g,
        float b,
        float a)
        : r(r),
          g(g),
          b(b),
          a(a)
    {
    }

    uint32_t Color::pack() const noexcept
    {
        auto toByte = [](float value) -> uint32_t
        {
            return static_cast<uint32_t>(
                std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
        };

        return (toByte(a) << 24) |
               (toByte(b) << 16) |
               (toByte(g) << 8) |
               toByte(r);
    }

}
