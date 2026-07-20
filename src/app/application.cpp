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
            int width = 0;
            int height = 0;

            if (m_window.consumeResize(width, height))
            {
                m_renderer.resize(
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height));
            }

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