#pragma once

namespace observe
{

    class Vec3
    {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        Vec3() = default;

        Vec3(
            float x,
            float y,
            float z);
    };

}