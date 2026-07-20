#pragma once

#include <cstdint>

#include <bgfx/bgfx.h>

#include <observe/shader.h>

namespace observe
{

    class ImGuiRenderer
    {
    public:
        ImGuiRenderer() = default;
        ~ImGuiRenderer();

        bool initialize();
        void shutdown();
        void render();

    private:
        bool m_initialized = false;

        uint16_t m_viewId = 1;

        bgfx::VertexLayout m_layout;
        bgfx::UniformHandle m_texUniform = BGFX_INVALID_HANDLE;

        Shader m_shader;
    };

}