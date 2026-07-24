#include "embedded/example_fonts.h"

#include <observe/app.h>
#include <observe/graphics.h>
#include <observe/ui.h>

observe::Path starPath(float center_x, float center_y, float radius)
{
  static constexpr float kPi = 3.14159265f;

  observe::Path path;
  int num_points = 10;
  for (int i = 0; i < num_points; ++i)
  {
    float angle = (float)i / (float)num_points * 2.0f * kPi;
    float r = (i % 2) ? radius : radius * 0.4f;
    auto point = observe::Point{center_x, center_y} +
                 r * observe::Point(std::sin(angle), std::cos(angle));
    if (i == 0)
      path.moveTo(point);
    else
      path.lineTo(point);
  }
  path.close();
  return path;
}

int runExample()
{
  observe::ApplicationWindow app;

  app.onDraw() = [&](observe::Canvas &canvas)
  {
    canvas.setColor(0xff222222);
    canvas.fill(0, 0, app.width(), app.height());

    canvas.setColor(0xffff44ff);

    float w = app.width() / 3.0f;
    float h = app.height();
    observe::Path star = starPath(w * 0.5f, h * 0.5f, std::min(w, h) * 0.4f);
    auto stroked = star.stroke(2);
    float segment = star.length() / 40.0f;
    auto dashed = star.stroke(2, observe::Path::Join::Round, observe::Path::EndCap::Round,
                              {segment}, canvas.time() * segment);
    canvas.fill(star);
    canvas.fill(stroked, w, 0);
    canvas.fill(dashed, 2.0f * w, 0);
    app.redraw();
  };

  app.setTitle("Observe Paths Example");
  app.show(900, 200);
  app.runEventLoop();
  return 0;
}
