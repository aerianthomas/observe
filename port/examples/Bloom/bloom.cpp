#include <observe/app.h>
#include <modules/observe_widgets/graph_line.h>

class AnimatedLine : public observe::Frame
{
public:
  static constexpr int kNumPoints = 1200;
  static constexpr float kDotRadius = 5.0f;

  AnimatedLine() : graph_line_(kNumPoints)
  {
    addChild(&graph_line_);
    setIgnoresMouseEvents(true, false);
  }

  void resized() override { graph_line_.setBounds(0, 0, width(), height()); }

  void setLinePositions(double render_time)
  {
    static constexpr float kPi = 3.14159265358979323846f;
    float position = 0.0f;
    double line_phase = render_time * 0.5;
    for (int i = 0; i < kNumPoints; ++i)
    {
      float t = 1.1f * i / (kNumPoints - 1.0f) - 0.05f;
      float delta = std::min(t, 1.0f - t);
      position += 0.02f * delta * delta + 0.003f;
      graph_line_.set(i, 0.5f + std::sin((line_phase + position) * 2.0f * kPi) * 0.25f);
    }
  }

  observe::Brush computeBrush(double render_time) const
  {
    observe::Gradient rainbow(0xffff6666, 0xffffff66, 0xff66ff66, 0xff66ffff, 0xff6666ff, 0xffff66ff,
                             0xffff6666);

    float boost_time = render_time * 0.2f;
    float boost_phase = (boost_time - floor(boost_time)) * 1.5f - 0.25f;
    observe::Gradient boost = observe::Gradient::fromSampleFunction(256, [boost_phase](float t)
                                                                  { return observe::Color(1.0f, 1.0f, 1.0f, 1.0f,
                                                                                         1.0f + std::max(0.0f, 0.4f - 3.0f * std::abs(boost_phase - t))); });

    return observe::Brush::linear(rainbow * boost, observe::Point(0, 0), observe::Point(width(), 0));
  }

  void draw(observe::Canvas &canvas) override
  {
    static constexpr int kNumDots = 10;

    double render_time = canvas.time();
    setLinePositions(render_time);
    observe::Brush brush = computeBrush(render_time);
    palette()->setColor(observe::GraphLine::LineColor, brush);
    canvas.setColor(brush);

    int render_height = height();
    int render_width = width();

    float center_y = render_height * 0.125f;
    for (int i = 0; i < kNumDots; ++i)
    {
      float t = (i + 1) / (kNumDots + 1.0f);
      float center_x = t * render_width;

      canvas.circle(center_x - kDotRadius, center_y - kDotRadius, kDotRadius * 2.0f);
      canvas.circle(center_x - kDotRadius, render_height - center_y - kDotRadius, kDotRadius * 2.0f);
    }

    redraw();
  }

private:
  observe::GraphLine graph_line_;
};

class ExampleEditor : public observe::ApplicationWindow
{
public:
  ExampleEditor()
  {
    bloom_.setBloomSize(30.0f);
    bloom_.setBloomIntensity(2.0f);
    setPostEffect(&bloom_);
    addChild(&animated_line_);
    animated_line_.layout().setMargin(0);

    onDraw() = [this](observe::Canvas &canvas)
    {
      canvas.setColor(0xff22282d);
      canvas.fill(0, 0, width(), height());
    };

    setPalette(&palette_);
    palette_.setValue(observe::GraphLine::LineWidth, 2.5f);
  }

private:
  observe::Palette palette_;
  observe::BloomPostEffect bloom_;
  AnimatedLine animated_line_;
};

int runExample()
{
  ExampleEditor editor;
  editor.setWindowDecoration(observe::Window::Decoration::Client);
  if (observe::isMobileDevice())
    editor.showMaximized();
  else
    editor.show(observe::Dimension::widthPercent(50.0f), observe::Dimension::widthPercent(14.0f));

  editor.runEventLoop();

  return 0;
}
