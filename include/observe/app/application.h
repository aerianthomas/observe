#pragma once

#include <vector>

#include <observe/windowing/window.h>
#include <observe/graphics/renderer.h>
#include <observe/app/application_layer.h>

namespace observe
{

    class Application
    {
    public:
        bool initialize();

        // Registers a layer to receive initialize()/update()/render()/
        // shutdown() calls. Must be called before Application::initialize().
        // Application does not own the layer - see ApplicationLayer's own
        // comment for lifetime rules.
        void addLayer(ApplicationLayer *layer);

        void run();

    private:
        Window m_window;
        Renderer m_renderer;

        std::vector<ApplicationLayer *> m_layers;
    };

}
