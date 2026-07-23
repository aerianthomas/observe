#pragma once

#include <cstdint>
#include <vector>

#include <bgfx/bgfx.h>

#include <observe/graphics/color.h>
#include <observe/graphics/rect.h>
#include <observe/graphics/shader.h>
#include <observe/math/vec2.h>

namespace observe
{

    // A standalone, immediate-style batched 2D draw surface: call the
    // draw*() methods to accumulate shapes, then render() once per frame
    // to submit them all as bgfx draw calls. Canvas has no knowledge of
    // widgets, layout, or any UI tree - it's the drawing primitive layer
    // those will eventually be built on top of, and is fully usable on
    // its own in the meantime.
    //
    // Rounded rects (and, by extension, circles and rounded-cap lines)
    // are drawn as a single plain quad per shape; the actual rounding is
    // computed per-pixel in the fragment shader via a signed distance
    // field, not tessellated geometry - see assets/shaders/fs_ui.sc.
    //
    // There's no clipping/scissor support yet - that's expected to arrive
    // alongside the widget tree/layout work, once there's an actual
    // parent-child hierarchy that needs it.
    class Canvas
    {
    public:
        Canvas() = default;
        ~Canvas();

        bool initialize();
        void shutdown();

        void drawRect(
            const Rect &rect,
            const Color &color);

        void drawRoundedRect(
            const Rect &rect,
            float radius,
            const Color &color);

        // 4-corner gradient. drawRect()/drawRoundedRect() are really just
        // this with all four corners the same color.
        void drawRectGradient(
            const Rect &rect,
            const Color &topLeft,
            const Color &topRight,
            const Color &bottomLeft,
            const Color &bottomRight);

        void drawCircle(
            const Vec2 &center,
            float radius,
            const Color &color);

        void drawLine(
            const Vec2 &start,
            const Vec2 &end,
            float thickness,
            const Color &color);

        // Submits everything accumulated since the last render() call as
        // bgfx draw calls, sized to (width, height) pixels, and clears the
        // draw list for the next frame. Call once per frame, after all
        // draw*() calls for that frame.
        void render(
            uint32_t width,
            uint32_t height);

    private:
        struct Vertex
        {
            float position[2];
            float localPosition[2];
            float shapeParams[4]; // x,y = half-size, z = radius, w = unused
            uint32_t color;
        };

        void addRoundedRectVertices(
            const Rect &rect,
            float radius,
            const Color &topLeft,
            const Color &topRight,
            const Color &bottomLeft,
            const Color &bottomRight);

        bool m_initialized = false;

        uint16_t m_viewId = 2; // 0 = Renderer's own scene, 1 = reserved for a future ImGui-style overlay layer

        bgfx::VertexLayout m_layout;
        Shader m_shader;

        std::vector<Vertex> m_vertices;
        std::vector<uint16_t> m_indices;
    };

}
