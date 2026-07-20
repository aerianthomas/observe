// #include <iostream>
#include <filesystem>

#include <observe/graphics/triangle.h>

namespace observe
{

    struct Vertex
    {
        float x;
        float y;
        float z;
    };

    bool Triangle::initialize()
    {
        /*std::cout
            << "Working directory: "
            << std::filesystem::current_path()
            << "\n";

        std::cout
            << "Vertex shader exists: "
            << std::filesystem::exists("shaders/vs_triangle.bin")
            << "\n";
        */

        Vertex vertices[] =
            {
                {0.0f, 0.5f, 0.0f},
                {0.5f, -0.5f, 0.0f},
                {-0.5f, -0.5f, 0.0f}};

        if (!m_mesh.create(
                vertices,
                3,
                m_layout))
        {
            // std::cout << "Mesh creation failed\n";
            return false;
        }

        if (!m_shader.load(
                "shaders/vs_triangle.bin",
                "shaders/fs_triangle.bin"))
        {
            // std::cout << "Shader loading failed\n";
            return false;
        }

        m_transform.setPosition(
            0.0f,
            0.0f,
            0.0f);

        m_transform.setRotation(
            0.0f,
            0.0f,
            0.0f);

        m_transform.setScale(
            2.0f,
            2.0f,
            2.0f);

        return true;
    }

    void Triangle::draw()
    {
        if (!m_shader.valid())
            return;

        bgfx::setTransform(
            m_transform.matrix().data());

        bgfx::setState(
            BGFX_STATE_WRITE_RGB |
            BGFX_STATE_WRITE_A |
            BGFX_STATE_MSAA);

        m_mesh.draw();

        bgfx::submit(
            0,
            m_shader.handle());
    }

}