#pragma once

#include <cstdint>

#include <bgfx/bgfx.h>

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

        bgfx::TextureHandle m_fontTexture = BGFX_INVALID_HANDLE;
    };

}