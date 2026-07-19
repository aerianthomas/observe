#pragma once

namespace observe
{

    class Mat4
    {
    public:
        Mat4();

        static Mat4 identity();

        static Mat4 translation(
            float x,
            float y,
            float z);

        static Mat4 rotationX(
            float radians);

        static Mat4 rotationY(
            float radians);

        static Mat4 rotationZ(
            float radians);

        static Mat4 scale(
            float x,
            float y,
            float z);

        Mat4 operator*(const Mat4 &other) const;

        float *data() noexcept
        {
            return m_data;
        }

        const float *data() const noexcept
        {
            return m_data;
        }

    private:
        float m_data[16];
    };

}