#include <observe/imgui/imgui_context.h>
#include <observe/windowing/window.h>

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

        ImGui::StyleColorsDark(); // added — otherwise you get 1.0-alpha-default styling

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

        ImGui::Render(); // added — without this GetDrawData() is stale/empty
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