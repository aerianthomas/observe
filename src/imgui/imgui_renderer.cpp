#include <observe/imgui_renderer.h>

#include <imgui.h>
#include <bgfx/bgfx.h>

namespace observe
{

    ImGuiRenderer::~ImGuiRenderer()
    {
        shutdown();
    }

    bool ImGuiRenderer::initialize()
    {
        unsigned char *pixels = nullptr;
        int width = 0;
        int height = 0;

        ImGuiIO &io = ImGui::GetIO();

        io.Fonts->GetTexDataAsRGBA32(
            &pixels,
            &width,
            &height);

        m_fontTexture =
            bgfx::createTexture2D(
                static_cast<uint16_t>(width),
                static_cast<uint16_t>(height),
                false,
                1,
                bgfx::TextureFormat::BGRA8,
                0,
                bgfx::copy(
                    pixels,
                    width * height * 4));

        if (!bgfx::isValid(m_fontTexture))
        {
            return false;
        }

        io.Fonts->SetTexID(
            (ImTextureID)(uintptr_t)m_fontTexture.idx);

        m_initialized = true;

        return true;
    }

    void ImGuiRenderer::render()
    {
        if (!m_initialized)
            return;
    }

    void ImGuiRenderer::shutdown()
    {
        if (!m_initialized)
            return;

        if (bgfx::isValid(m_fontTexture))
        {
            bgfx::destroy(m_fontTexture);
            m_fontTexture = BGFX_INVALID_HANDLE;
        }

        m_initialized = false;
    }

}