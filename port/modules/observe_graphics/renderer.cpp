#include "renderer.h"

#include "modules/observe_utils/string_utils.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace observe
{
  class GraphicsCallbackHandler : public bgfx::CallbackI
  {
    void fatal(const char *file_path, uint16_t line, bgfx::Fatal::Enum _code, const char *error) override
    {
      OBSERVE_LOG(String(file_path) + String(" (") + line + String(") "));
      OBSERVE_LOG(error);
      OBSERVE_LOG("Graphics fatal error");
      OBSERVE_ASSERT(false);
    }

    void traceVargs(const char *file_path, uint16_t line, const char *format, va_list arg_list) override
    {
#if OBSERVE_GRAPHICS_DEBUG_LOGGING
      observe::debugLogArgs(file_path, line, format, arg_list);
#endif
    }

    void profilerBegin(const char *, uint32_t, const char *, uint16_t) override {}
    void profilerBeginLiteral(const char *, uint32_t, const char *, uint16_t) override {}
    void profilerEnd() override {}
    uint32_t cacheReadSize(uint64_t) override { return 0; }
    bool cacheRead(uint64_t, void *, uint32_t) override { return false; }
    void cacheWrite(uint64_t, const void *, uint32_t) override {}

    void screenShot(const char *file_path, uint32_t width, uint32_t height, uint32_t pitch,
                    const void *data, uint32_t size, bool y_flip) override
    {
      Renderer::instance().setScreenshotData(static_cast<const uint8_t *>(data), width, height, pitch, true);
    }

    void captureBegin(uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, bool) override {}
    void captureEnd() override {}
    void captureFrame(const void *, uint32_t) override {}
  };

  static constexpr uint32_t resetFlags()
  {
#if OBSERVE_WINDOWS
    return BGFX_RESET_FLIP_AFTER_RENDER;
#elif OBSERVE_MAC
    return BGFX_RESET_FLIP_AFTER_RENDER | BGFX_RESET_VSYNC;
#elif OBSERVE_LINUX
    return BGFX_RESET_VSYNC;
#else
    return 0;
#endif
  }

  Renderer &Renderer::instance()
  {
    static Renderer renderer;
    return renderer;
  }

  Renderer::Renderer() : Thread("Renderer Thread") {}

  Renderer::~Renderer()
  {
    stop();
  }

  void Renderer::startRenderThread()
  {
#if OBSERVE_BACKGROUND_GRAPHICS_THREAD
    start();
    while (!render_thread_started_.load())
      yield();
#endif
  }

  void Renderer::run()
  {
    render();
  }

  void Renderer::render()
  {
    static constexpr int kRenderTimeout = 100;
    render_thread_started_ = bgfx::renderFrame() == bgfx::RenderFrame::NoContext;

    while (shouldRun())
      bgfx::renderFrame(kRenderTimeout);
  }

  void Renderer::initialize(void *model_window, void *display)
  {
    if (initialized_)
      return;

    callback_handler_ = std::make_unique<GraphicsCallbackHandler>();
    initialized_ = true;
    startRenderThread();

    bgfx::Init bgfx_init;
    bgfx_init.resolution.numBackBuffers = 1;
    bgfx_init.resolution.width = 0;
    bgfx_init.resolution.height = 0;
    bgfx_init.callback = callback_handler_.get();

    bgfx_init.platformData.ndt = display;
    bgfx_init.platformData.nwh = model_window;
    bgfx_init.platformData.type = bgfx::NativeWindowHandleType::Default;

    bgfx::RendererType::Enum supported_renderers[bgfx::RendererType::Count];
    uint8_t num_supported = bgfx::getSupportedRenderers(bgfx::RendererType::Count, supported_renderers);

#if OBSERVE_WINDOWS
    bgfx_init.type = bgfx::RendererType::Direct3D11;
#if USE_DIRECTX12
    for (int i = 0; i < num_supported; ++i)
    {
      if (supported_renderers[i] == bgfx::RendererType::Direct3D12)
        bgfx_init.type = bgfx::RendererType::Direct3D12;
    }
#endif
#elif OBSERVE_MAC
    bgfx_init.type = bgfx::RendererType::Metal;
    bgfx_init.resolution.width = 1;
    bgfx_init.resolution.height = 1;
#elif OBSERVE_LINUX
    bgfx_init.type = bgfx::RendererType::Vulkan;
#elif OBSERVE_EMSCRIPTEN
    bgfx_init.type = bgfx::RendererType::OpenGLES;
#endif

    bgfx_init.resolution.reset = resetFlags();

    for (int i = 0; i < num_supported && !supported_; ++i)
      supported_ = supported_renderers[i] == bgfx_init.type;

    if (!supported_)
    {
      OBSERVE_ASSERT(false);
      std::string renderer_name = bgfx::getRendererName(bgfx_init.type);
      error_message_ = renderer_name + " is required and not supported on this computer.";
    }

    bgfx::init(bgfx_init);
    OBSERVE_ASSERT(bgfx::getRendererType() == bgfx_init.type);
    swap_chain_supported_ = bgfx::getCaps()->supported & BGFX_CAPS_SWAP_CHAIN;
  }

  void Renderer::resetResolution(int width, int height)
  {
#if OBSERVE_MAC
    bgfx::reset(width, height, resetFlags());
#endif
  }

  void Renderer::setScreenshotData(const uint8_t *data, int width, int height, int pitch, bool blue_red)
  {
    screenshot_ = Screenshot(data, width, height, pitch, blue_red);
  }
}
