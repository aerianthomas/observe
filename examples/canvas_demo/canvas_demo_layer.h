#pragma once

#include <observe/app/application_layer.h>
#include <observe/ui/canvas.h>

namespace canvas_demo
{

    class CanvasDemoLayer : public observe::ApplicationLayer
    {
    public:
        bool initialize(observe::Window &window) override;
        void shutdown() override;

        void update() override;
        void render() override;

    private:
        observe::Window *m_window = nullptr;

        observe::Canvas m_canvas;
    };

}
