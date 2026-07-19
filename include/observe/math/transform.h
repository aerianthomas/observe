#pragma once

#include <observe/math/mat4.h>
#include <observe/math/vec3.h>

namespace observe
{

    class Transform
    {
    public:
        Transform();

        void setPosition(
            float x,
            float y,
            float z);

        void setRotation(
            float x,
            float y,
            float z);

        void setScale(
            float x,
            float y,
            float z);

        const Mat4 &matrix() const noexcept
        {
            return m_matrix;
        }

    private:
        Vec3 m_position;
        Vec3 m_rotation;
        Vec3 m_scale;

        Mat4 m_matrix;

        void updateMatrix();
    };

}