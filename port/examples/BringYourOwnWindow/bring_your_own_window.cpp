#include <observe/graphics.h>
#include <observe/windowing.h>

int runExample()
{
  std::unique_ptr<observe::Window> window = observe::createWindow(800, 800);
  observe::Renderer::instance().initialize(window->initWindow(), window->globalDisplay());

  observe::Canvas canvas;
  canvas.pairToWindow(window->nativeHandle(), window->clientWidth(), window->clientHeight());
  canvas.setColor(0xff223333);
  canvas.fill(0, 0, window->clientWidth(), window->clientHeight());
  canvas.setColor(0xffaa99ff);
  canvas.ring(50, 50, window->clientWidth() - 100.0f, window->clientWidth() * 0.1f);

  window->show();
  canvas.submit();

  window->runEventLoop();

  return 0;
}
