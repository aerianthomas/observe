#pragma once

#include <cstdint>

#include <observe/graphics/triangle.h>
#include <observe/graphics/camera.h>

namespace observe
{

    class Window;

    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer();

        bool initialize(const Window &window);

        void resize(uint32_t width, uint32_t height);

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