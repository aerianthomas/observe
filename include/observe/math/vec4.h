#pragma once

namespace observe
{

    class Vec4
    {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;

        Vec4() = default;

        Vec4(
            float x,
            float y,
            float z,
            float w);
    };

}