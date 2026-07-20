#include <observe/app/application.h>
#include <observe/graphics/renderer.h>
#include <imgui.h>

// #include <iostream>

namespace observe
{

    bool Application::initialize()
    {
        if (!m_window.create(
                "Observe",
                1280,
                720))
        {
            return false;
        }

        if (!m_renderer.initialize(m_window))
        {
            return false;
        }

        if (!m_imgui.initialize(m_window))
        {
            return false;
        }

        if (!m_imguiRenderer.initialize())
        {
            return false;
        }

        return true;
    }

    void Application::run()
    {
        while (m_window.pollEvents())
        {
            m_imgui.beginFrame();

            ImGui::ShowDemoWindow(); // <- proves the pipeline works end-to-end

            m_renderer.beginFrame();

            m_imgui.endFrame();

            m_imguiRenderer.render();

            m_renderer.endFrame();
        }

        // std::cout << "run loop exited\n";
    }

}