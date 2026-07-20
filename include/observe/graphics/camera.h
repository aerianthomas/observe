#pragma once

#include <observe/math/mat4.h>

namespace observe
{

    class Camera
    {
    public:
        Camera();

        void resize(
            float width,
            float height);

        const Mat4 &view() const noexcept
        {
            return m_view;
        }

        const Mat4 &projection() const noexcept
        {
            return m_projection;
        }

    private:
        Mat4 m_view;
        Mat4 m_projection;

        float m_fov = 60.0f;
        float m_near = 0.1f;
        float m_far = 100.0f;
    };

}