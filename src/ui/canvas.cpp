#include <observe/ui/canvas.h>

#include <bx/math.h>

#include <cmath>
#include <cstring>
#include <algorithm>

namespace observe
{

    Canvas::~Canvas()
    {
        shutdown();
    }

    bool Canvas::initialize()
    {
        m_layout
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord1, 4, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

        if (!m_shader.load(
                "shaders/vs_ui.bin",
                "shaders/fs_ui.bin"))
        {
            return false;
        }

        m_initialized = true;

        return true;
    }

    void Canvas::shutdown()
    {
        if (!m_initialized)
            return;

        m_shader.destroy();

        m_vertices.clear();
        m_indices.clear();

        m_initialized = false;
    }

    void Canvas::addRoundedRectVertices(
        const Rect &rect,
        float radius,
        const Color &topLeft,
        const Color &topRight,
        const Color &bottomLeft,
        const Color &bottomRight)
    {
        const Vec2 center = rect.center();
        const Vec2 halfSize = rect.halfSize();

        const float clampedRadius =
            std::min(
                radius,
                std::min(halfSize.x, halfSize.y));

        const uint16_t base = static_cast<uint16_t>(m_vertices.size());

        auto addVertex = [&](float px, float py, const Color &color)
        {
            Vertex vertex{};

            vertex.position[0] = px;
            vertex.position[1] = py;

            vertex.localPosition[0] = px - center.x;
            vertex.localPosition[1] = py - center.y;

            vertex.shapeParams[0] = halfSize.x;
            vertex.shapeParams[1] = halfSize.y;
            vertex.shapeParams[2] = clampedRadius;
            vertex.shapeParams[3] = 0.0f;

            vertex.color = color.pack();

            m_vertices.push_back(vertex);
        };

        addVertex(rect.x, rect.y, topLeft);
        addVertex(rect.x + rect.width, rect.y, topRight);
        addVertex(rect.x, rect.y + rect.height, bottomLeft);
        addVertex(rect.x + rect.width, rect.y + rect.height, bottomRight);

        m_indices.push_back(base + 0);
        m_indices.push_back(base + 1);
        m_indices.push_back(base + 2);
        m_indices.push_back(base + 2);
        m_indices.push_back(base + 1);
        m_indices.push_back(base + 3);
    }

    void Canvas::drawRect(
        const Rect &rect,
        const Color &color)
    {
        addRoundedRectVertices(rect, 0.0f, color, color, color, color);
    }

    void Canvas::drawRoundedRect(
        const Rect &rect,
        float radius,
        const Color &color)
    {
        addRoundedRectVertices(rect, radius, color, color, color, color);
    }

    void Canvas::drawRectGradient(
        const Rect &rect,
        const Color &topLeft,
        const Color &topRight,
        const Color &bottomLeft,
        const Color &bottomRight)
    {
        addRoundedRectVertices(rect, 0.0f, topLeft, topRight, bottomLeft, bottomRight);
    }

    void Canvas::drawCircle(
        const Vec2 &center,
        float radius,
        const Color &color)
    {
        const Rect rect(
            center.x - radius,
            center.y - radius,
            radius * 2.0f,
            radius * 2.0f);

        addRoundedRectVertices(rect, radius, color, color, color, color);
    }

    void Canvas::drawLine(
        const Vec2 &start,
        const Vec2 &end,
        float thickness,
        const Color &color)
    {
        Vec2 direction(end.x - start.x, end.y - start.y);

        const float length = std::sqrt(
            direction.x * direction.x + direction.y * direction.y);

        if (length < 1e-5f)
            return;

        direction.x /= length;
        direction.y /= length;

        // Perpendicular to the line direction, for the thickness offset.
        const Vec2 normal(-direction.y, direction.x);

        const Vec2 center(
            (start.x + end.x) * 0.5f,
            (start.y + end.y) * 0.5f);

        const float halfLength = length * 0.5f;
        const float halfThickness = thickness * 0.5f;
        const float radius = halfThickness; // rounded end caps

        const uint16_t base = static_cast<uint16_t>(m_vertices.size());

        // Corners are built in the line's own local space (unrotated):
        // +/- halfLength along its direction, +/- halfThickness across
        // it. Screen-space position is those same local offsets rotated
        // by (direction, normal) and translated to the line's center -
        // the fragment shader only ever sees the local (unrotated) frame,
        // exactly like it does for an axis-aligned rect.
        auto addVertex = [&](float alongLocal, float acrossLocal)
        {
            Vertex vertex{};

            const float px = center.x + direction.x * alongLocal + normal.x * acrossLocal;
            const float py = center.y + direction.y * alongLocal + normal.y * acrossLocal;

            vertex.position[0] = px;
            vertex.position[1] = py;

            vertex.localPosition[0] = alongLocal;
            vertex.localPosition[1] = acrossLocal;

            vertex.shapeParams[0] = halfLength;
            vertex.shapeParams[1] = halfThickness;
            vertex.shapeParams[2] = radius;
            vertex.shapeParams[3] = 0.0f;

            vertex.color = color.pack();

            m_vertices.push_back(vertex);
        };

        addVertex(-halfLength, -halfThickness);
        addVertex(halfLength, -halfThickness);
        addVertex(-halfLength, halfThickness);
        addVertex(halfLength, halfThickness);

        m_indices.push_back(base + 0);
        m_indices.push_back(base + 1);
        m_indices.push_back(base + 2);
        m_indices.push_back(base + 2);
        m_indices.push_back(base + 1);
        m_indices.push_back(base + 3);
    }

    void Canvas::render(
        uint32_t width,
        uint32_t height)
    {
        if (!m_initialized)
            return;

        if (m_vertices.empty())
            return;

        if (width == 0 || height == 0)
        {
            m_vertices.clear();
            m_indices.clear();
            return;
        }

        {
            float ortho[16];

            bx::mtxOrtho(
                ortho,
                0.0f,
                static_cast<float>(width),
                static_cast<float>(height),
                0.0f,
                0.0f,
                1000.0f,
                0.0f,
                bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(m_viewId, nullptr, ortho);

            bgfx::setViewRect(
                m_viewId,
                0,
                0,
                static_cast<uint16_t>(width),
                static_cast<uint16_t>(height));
        }

        const uint32_t numVertices = static_cast<uint32_t>(m_vertices.size());
        const uint32_t numIndices = static_cast<uint32_t>(m_indices.size());

        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        if (bgfx::allocTransientBuffers(&tvb, m_layout, numVertices, &tib, numIndices))
        {
            std::memcpy(tvb.data, m_vertices.data(), numVertices * sizeof(Vertex));
            std::memcpy(tib.data, m_indices.data(), numIndices * sizeof(uint16_t));

            bgfx::setState(
                BGFX_STATE_WRITE_RGB |
                BGFX_STATE_WRITE_A |
                BGFX_STATE_MSAA |
                BGFX_STATE_BLEND_FUNC(
                    BGFX_STATE_BLEND_SRC_ALPHA,
                    BGFX_STATE_BLEND_INV_SRC_ALPHA));

            bgfx::setVertexBuffer(0, &tvb, 0, numVertices);
            bgfx::setIndexBuffer(&tib, 0, numIndices);

            bgfx::submit(m_viewId, m_shader.handle());
        }

        m_vertices.clear();
        m_indices.clear();
    }

}
