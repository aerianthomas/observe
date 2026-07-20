#include <observe/graphics/camera.h>

#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace observe
{

    Camera::Camera()
    {
        bx::mtxLookAt(
            m_view.data(),
            {0.0f, 0.0f, -3.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f});
    }

    void Camera::resize(
        float width,
        float height)
    {
        bx::mtxProj(
            m_projection.data(),
            m_fov,
            width / height,
            m_near,
            m_far,
            bgfx::getCaps()->homogeneousDepth);
    }

}