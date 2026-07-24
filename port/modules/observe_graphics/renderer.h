#pragma once

#include "screenshot.h"
#include "modules/observe_utils/thread_utils.h"
#include "windowless_context.h"

namespace observe
{
  class GraphicsCallbackHandler;

  class Renderer : public Thread
  {
  public:
    static Renderer &instance();

    Renderer();
    ~Renderer() override;

    static void resetResolution(int width, int height);

    void initializeWindowless() { initialize(windowlessContext(), nullptr); }
    void initialize(void *model_window, void *display);
    void setScreenshotData(const uint8_t *data, int width, int height, int pitch, bool blue_red);
    const Screenshot &screenshot() const { return screenshot_; }

    const std::string &errorMessage() const { return error_message_; }
    bool supported() const { return supported_; }
    bool swapChainSupported() const { return swap_chain_supported_; }
    bool initialized() const { return initialized_; }

  private:
    void startRenderThread();
    void render();
    void run() override;

    bool initialized_ = false;
    bool supported_ = false;
    bool swap_chain_supported_ = false;

    Screenshot screenshot_;
    std::string error_message_;
    std::atomic<bool> render_thread_started_ = false;
    std::unique_ptr<GraphicsCallbackHandler> callback_handler_;
  };
}
