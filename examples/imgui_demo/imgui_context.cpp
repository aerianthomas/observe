#include "imgui_context.h"

#include <observe/windowing/window.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>

namespace imgui_demo
{

    ImGuiContext::~ImGuiContext()
    {
        shutdown();
    }

    bool ImGuiContext::initialize(
        observe::Window &window)
    {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGui::StyleColorsDark(); // added — otherwise you get 1.0-alpha-default styling

        if (!ImGui_ImplSDL3_InitForOther(
                window.handle()))
        {
            return false;
        }

        // Window has no idea ImGui exists - it just calls every
        // registered listener for every SDL event it sees. This is what
        // used to be a hardcoded ImGui_ImplSDL3_ProcessEvent() call
        // inside Window::pollEvents() itself.
        window.addEventListener(
            [](const SDL_Event &event)
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
            });

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
