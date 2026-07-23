#pragma once

#include <cstdint>

namespace observe
{

    class Color
    {
    public:
        float r = 1.0f;
        float g = 1.0f;
        float b = 1.0f;
        float a = 1.0f;

        Color() = default;

        Color(
            float r,
            float g,
            float b,
            float a = 1.0f);

        // Packs to the byte order Canvas's vertex color attribute expects
        // (matches the convention used for bgfx::AttribType::Uint8
        // normalized colors elsewhere in observe).
        [[nodiscard]]
        uint32_t pack() const noexcept;
    };

}
