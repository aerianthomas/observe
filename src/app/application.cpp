#include <observe/app/application.h>

// #include <iostream>

namespace observe
{

    void Application::addLayer(ApplicationLayer *layer)
    {
        m_layers.push_back(layer);
    }

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

        for (ApplicationLayer *layer : m_layers)
        {
            if (!layer->initialize(m_window))
            {
                return false;
            }
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

            for (ApplicationLayer *layer : m_layers)
                layer->update();

            m_renderer.beginFrame();

            for (ApplicationLayer *layer : m_layers)
                layer->render();

            m_renderer.endFrame();
        }

        for (ApplicationLayer *layer : m_layers)
            layer->shutdown();

        // std::cout << "run loop exited\n";
    }

}
