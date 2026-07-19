#include <observe/math/transform.h>

namespace observe
{

    Transform::Transform()
        : m_position(0.0f, 0.0f, 0.0f),
          m_rotation(0.0f, 0.0f, 0.0f),
          m_scale(1.0f, 1.0f, 1.0f),
          m_matrix(Mat4::identity())
    {
    }

    void Transform::setPosition(
        float x,
        float y,
        float z)
    {
        m_position.x = x;
        m_position.y = y;
        m_position.z = z;

        updateMatrix();
    }

    void Transform::setRotation(
        float x,
        float y,
        float z)
    {
        m_rotation.x = x;
        m_rotation.y = y;
        m_rotation.z = z;

        updateMatrix();
    }

    void Transform::setScale(
        float x,
        float y,
        float z)
    {
        m_scale.x = x;
        m_scale.y = y;
        m_scale.z = z;

        updateMatrix();
    }

    void Transform::updateMatrix()
    {
        Mat4 translation =
            Mat4::translation(
                m_position.x,
                m_position.y,
                m_position.z);

        Mat4 rotationX =
            Mat4::rotationX(
                m_rotation.x);

        Mat4 rotationY =
            Mat4::rotationY(
                m_rotation.y);

        Mat4 rotationZ =
            Mat4::rotationZ(
                m_rotation.z);

        Mat4 scale =
            Mat4::scale(
                m_scale.x,
                m_scale.y,
                m_scale.z);

        m_matrix =
            translation *
            rotationZ *
            rotationY *
            rotationX *
            scale;
    }

}