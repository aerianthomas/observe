#include <observe/app.h>

int runExample()
{
  observe::ApplicationWindow app;

  app.onDraw() = [&app](observe::Canvas &canvas)
  {
    canvas.setColor(0xff000066);
    canvas.fill(0, 0, app.width(), app.height());

    float circle_radius = app.height() * 0.1f;
    float x = app.width() * 0.5f - circle_radius;
    float y = app.height() * 0.5f - circle_radius;
    canvas.setColor(0xff00ffff);
    canvas.circle(x, y, 2.0f * circle_radius);
  };

  app.setTitle("Observe Basic Example");
  app.show(800, 600);
  app.runEventLoop();
  return 0;
}
