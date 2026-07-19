#pragma once

#include <cstdint>

#include <observe/triangle.h>
#include <observe/camera.h>

namespace observe
{

    class Window;

    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer();

        bool initialize(const Window &window);

        void beginFrame();
        void endFrame();

        void shutdown();

    private:
        bool m_initialized = false;

        uint32_t m_width = 0;
        uint32_t m_height = 0;

        Triangle m_triangle;

        Camera m_camera;
    };

}