#include <observe/application.h>
#include <observe/renderer.h>
#include <imgui.h>

#include <iostream>

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
            // std::cout << "frame\n";

            m_imgui.beginFrame();

            m_renderer.beginFrame();

            m_imgui.endFrame();

            m_imguiRenderer.render();

            m_renderer.endFrame();
        }

        std::cout << "run loop exited\n";
    }

}