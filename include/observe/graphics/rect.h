#pragma once

#include <observe/math/vec2.h>

namespace observe
{

    class Rect
    {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;

        Rect() = default;

        Rect(
            float x,
            float y,
            float width,
            float height);

        [[nodiscard]]
        Vec2 center() const noexcept;

        [[nodiscard]]
        Vec2 halfSize() const noexcept;
    };

}
