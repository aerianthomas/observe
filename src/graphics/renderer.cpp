#include <observe/renderer.h>
#include <observe/window.h>

#include <bgfx/bgfx.h>

namespace observe
{

    Renderer::~Renderer()
    {
        shutdown();
    }

    bool Renderer::initialize(const Window &window)
    {
        m_width = static_cast<uint32_t>(window.width());
        m_height = static_cast<uint32_t>(window.height());

        bgfx::Init init{};

        init.type = bgfx::RendererType::Count;

        init.platformData.nwh = window.nativeHandle();

        init.resolution.width = m_width;
        init.resolution.height = m_height;
        init.resolution.reset = BGFX_RESET_VSYNC;

        if (!bgfx::init(init))
            return false;

        bgfx::setViewClear(
            0,
            BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
            0x202040ff,
            1.0f,
            0);

        bgfx::setViewRect(
            0,
            0,
            0,
            m_width,
            m_height);

        m_camera.resize(
            static_cast<float>(window.width()),
            static_cast<float>(window.height()));

        if (!m_triangle.initialize())
        {
            return false;
        }

        m_initialized = true;

        return true;
    }

    void Renderer::beginFrame()
    {
        if (!m_initialized)
            return;

        bgfx::setViewTransform(
            0,
            m_camera.view().data(),
            m_camera.projection().data());

        bgfx::setViewRect(
            0,
            0,
            0,
            m_width,
            m_height);

        bgfx::touch(0);

        m_triangle.draw();
    }

    void Renderer::endFrame()
    {
        if (!m_initialized)
            return;

        bgfx::frame();
    }

    void Renderer::shutdown()
    {
        if (!m_initialized)
            return;

        bgfx::shutdown();

        m_initialized = false;
    }

}