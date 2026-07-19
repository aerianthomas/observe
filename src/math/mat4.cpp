#include <observe/math/mat4.h>

#include <cmath>

namespace observe
{

    Mat4::Mat4()
    {
        for (int i = 0; i < 16; ++i)
            m_data[i] = 0.0f;
    }

    Mat4 Mat4::identity()
    {
        Mat4 result;

        result.m_data[0] = 1.0f;
        result.m_data[5] = 1.0f;
        result.m_data[10] = 1.0f;
        result.m_data[15] = 1.0f;

        return result;
    }

    Mat4 Mat4::translation(
        float x,
        float y,
        float z)
    {
        Mat4 result = Mat4::identity();

        result.m_data[12] = x;
        result.m_data[13] = y;
        result.m_data[14] = z;

        return result;
    }

    Mat4 Mat4::rotationX(
        float radians)
    {
        Mat4 result = Mat4::identity();

        float c = std::cos(radians);
        float s = std::sin(radians);

        result.m_data[5] = c;
        result.m_data[6] = s;
        result.m_data[9] = -s;
        result.m_data[10] = c;

        return result;
    }

    Mat4 Mat4::rotationY(
        float radians)
    {
        Mat4 result = Mat4::identity();

        float c = std::cos(radians);
        float s = std::sin(radians);

        result.m_data[0] = c;
        result.m_data[2] = -s;
        result.m_data[8] = s;
        result.m_data[10] = c;

        return result;
    }

    Mat4 Mat4::rotationZ(
        float radians)
    {
        Mat4 result = Mat4::identity();

        float c = std::cos(radians);
        float s = std::sin(radians);

        result.m_data[0] = c;
        result.m_data[1] = s;
        result.m_data[4] = -s;
        result.m_data[5] = c;

        return result;
    }

    Mat4 Mat4::scale(
        float x,
        float y,
        float z)
    {
        Mat4 result = Mat4::identity();

        result.data()[0] = x;
        result.data()[5] = y;
        result.data()[10] = z;

        return result;
    }

    Mat4 Mat4::operator*(const Mat4 &other) const
    {
        Mat4 result;

        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                result.m_data[col * 4 + row] =
                    m_data[0 * 4 + row] * other.m_data[col * 4 + 0] +
                    m_data[1 * 4 + row] * other.m_data[col * 4 + 1] +
                    m_data[2 * 4 + row] * other.m_data[col * 4 + 2] +
                    m_data[3 * 4 + row] * other.m_data[col * 4 + 3];
            }
        }

        return result;
    }

}