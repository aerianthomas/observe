#include "embedded/example_shaders.h"

#include <complex>
#include <observe/app.h>
#include <modules/observe_widgets/shader_editor.h>

using namespace observe::dimension;

static void drawRing(observe::Canvas &canvas, int width, int height, float radius,
                     float circle_diameter, int num, float phase_offset)
{
  static constexpr float kPi = 3.14159265358979323846f;

  float phase_inc = 2.0f * kPi / num;
  std::complex<float> tick(cosf(phase_inc), sinf(phase_inc));
  std::complex<float> position(radius * cosf(phase_offset), radius * sinf(phase_offset));
  float center_x = (width - circle_diameter) / 2.0f;
  float center_y = (height - circle_diameter) / 2.0f;

  for (int i = 0; i < num; ++i)
  {
    position *= tick;
    canvas.circle(center_x + position.real(), center_y + position.imag(), circle_diameter);
  }
}

static void drawRotatingCircles(observe::Canvas &canvas, int width, int height)
{
  static constexpr int kIncrement = 6;
  static constexpr int kNumRings = 20;

  canvas.setColor(0xffffffff);
  float radius_increment = height * 0.5f / kNumRings;
  float circle_diameter = height * 0.4f / kNumRings;
  float phase_offset = canvas.time() * 0.03f;
  for (int i = 0; i < kNumRings; ++i)
  {
    drawRing(canvas, width, height, i * radius_increment, circle_diameter, i * kIncrement,
             phase_offset * (kNumRings - i));
  }
}

class ExampleEditor : public observe::ApplicationWindow
{
public:
  ExampleEditor()
  {
    shapes_.onDraw() = [this](observe::Canvas &canvas)
    {
      drawRotatingCircles(canvas, shapes_.width(), shapes_.height());
      shapes_.redraw();
    };

    post_effect_ = std::make_unique<observe::ShaderPostEffect>(resources::shaders::vs_custom,
                                                              resources::shaders::fs_warp);

    shader_editor_.setShader(resources::shaders::fs_warp, resources::shaders::fs_warp_sc);

    shapes_.setPostEffect(post_effect_.get());
    addChild(&shapes_);
    addChild(&shader_editor_);
  }

  void draw(observe::Canvas &canvas) override
  {
    canvas.setColor(0xff000000);
    canvas.fill(0, 0, shader_editor_.x(), height());
  }

  void resized() override
  {
    float center = width() / 2.0f;
    int shapes_width = std::min(center, height());
    shapes_.setBounds((center - shapes_width) / 2, (height() - shapes_width) / 2, shapes_width, shapes_width);
    shader_editor_.setBounds(center, 0, width() - center, height());
  }

private:
  Frame shapes_;
  std::unique_ptr<observe::ShaderPostEffect> post_effect_;
  observe::ShaderEditor shader_editor_;
};

int runExample()
{
  ExampleEditor editor;
  editor.showMaximized();
  editor.runEventLoop();

  return 0;
}
