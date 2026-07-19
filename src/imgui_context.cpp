#include <observe/imgui_context.h>
#include <observe/window.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>

namespace observe
{

    ImGuiContext::~ImGuiContext()
    {
        shutdown();
    }

    bool ImGuiContext::initialize(
        Window &window)
    {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        if (!ImGui_ImplSDL3_InitForOther(
                window.handle()))
        {
            return false;
        }

        m_initialized = true;

        return true;
    }

    void ImGuiContext::beginFrame()
    {
        if (!m_initialized)
            return;

        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiContext::endFrame()
    {
        if (!m_initialized)
            return;
    }

    void ImGuiContext::shutdown()
    {
        if (!m_initialized)
            return;

        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        m_initialized = false;
    }

}