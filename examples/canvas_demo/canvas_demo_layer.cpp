#include "canvas_demo_layer.h"

#include <observe/windowing/window.h>

namespace canvas_demo
{

    bool CanvasDemoLayer::initialize(observe::Window &window)
    {
        m_window = &window;

        return m_canvas.initialize();
    }

    void CanvasDemoLayer::shutdown()
    {
        m_canvas.shutdown();
    }

    void CanvasDemoLayer::update()
    {
        // A quick visual proof of each primitive - nothing here is meant
        // to look good, just to confirm every draw call actually renders.

        m_canvas.drawRect(
            observe::Rect(40.0f, 40.0f, 160.0f, 100.0f),
            observe::Color(0.85f, 0.25f, 0.25f, 1.0f));

        m_canvas.drawRoundedRect(
            observe::Rect(240.0f, 40.0f, 160.0f, 100.0f),
            18.0f,
            observe::Color(0.25f, 0.65f, 0.85f, 1.0f));

        m_canvas.drawRectGradient(
            observe::Rect(440.0f, 40.0f, 160.0f, 100.0f),
            observe::Color(0.95f, 0.55f, 0.15f, 1.0f),
            observe::Color(0.95f, 0.15f, 0.55f, 1.0f),
            observe::Color(0.15f, 0.55f, 0.95f, 1.0f),
            observe::Color(0.55f, 0.15f, 0.95f, 1.0f));

        m_canvas.drawCircle(
            observe::Vec2(120.0f, 240.0f),
            50.0f,
            observe::Color(0.35f, 0.85f, 0.45f, 1.0f));

        m_canvas.drawLine(
            observe::Vec2(240.0f, 190.0f),
            observe::Vec2(560.0f, 290.0f),
            10.0f,
            observe::Color(0.9f, 0.9f, 0.2f, 1.0f));
    }

    void CanvasDemoLayer::render()
    {
        if (!m_window)
            return;

        m_canvas.render(
            static_cast<uint32_t>(m_window->width()),
            static_cast<uint32_t>(m_window->height()));
    }

}
