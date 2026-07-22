#include "imgui_layer.h"

#include <imgui.h>

namespace imgui_demo
{

    bool ImGuiLayer::initialize(observe::Window &window)
    {
        if (!m_context.initialize(window))
            return false;

        if (!m_renderer.initialize())
            return false;

        return true;
    }

    void ImGuiLayer::shutdown()
    {
        // Same ordering reasoning as the declaration order in the header:
        // the renderer needs the context still alive to clean up textures.
        m_renderer.shutdown();
        m_context.shutdown();
    }

    void ImGuiLayer::update()
    {
        m_context.beginFrame();

        ImGui::ShowDemoWindow(); // proves the pipeline works end-to-end
    }

    void ImGuiLayer::render()
    {
        m_context.endFrame();

        m_renderer.render();
    }

}
