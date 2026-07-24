/// Custom gradient definitions:
///
/// OkLab: https://bottosson.github.io/posts/oklab/
/// Viridis: https://sjmgarnier.github.io/viridis/articles/intro-to-viridis.html

#include "embedded/example_fonts.h"

#include <array>
#include <observe/app.h>
#include <observe/widgets.h>

using namespace observe::dimension;

observe::Color sampleOkLab(float t)
{
  static constexpr float kPi = 3.14159265358979323846f;
  static constexpr float kL = 0.82f;
  static constexpr float kC = 0.15f;
  static constexpr float kOffset = 0.45f;

  float a = kC * std::cos(2.0f * kPi * t + kOffset);
  float b = kC * std::sin(2.0f * kPi * t + kOffset);

  float l_ = kL + 0.3963377774f * a + 0.2158037573f * b;
  float m_ = kL - 0.1055613458f * a - 0.0638541728f * b;
  float s_ = kL - 0.0894841775f * a - 1.2914855480f * b;

  float l = l_ * l_ * l_;
  float m = m_ * m_ * m_;
  float s = s_ * s_ * s_;

  return {1.0f, +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
          -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
          -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s};
}

void setPattern(observe::Gradient &gradient, const std::string &pattern)
{
  if (pattern == "Single")
  {
    gradient.setRepeat(false);
    gradient.setReflect(false);
  }
  else if (pattern == "Repeat")
  {
    gradient.setRepeat(true);
    gradient.setReflect(false);
  }
  else
  {
    gradient.setRepeat(false);
    gradient.setReflect(true);
  }
}

class PointsFrame : public observe::Frame
{
public:
  static constexpr int kDragRadius = 20;
  static constexpr int kDotRadius = 5;
  enum ActivePoint
  {
    kNone,
    kFrom,
    kTo
  };

  void resized() override
  {
    point1_ = observe::Point(width() * 0.33f, height() * 0.33f);
    point2_ = observe::Point(width() * 0.66f, height() * 0.66f);
  }

  void draw(observe::Canvas &canvas) override
  {
    observe::Brush points = observe::Brush::linear(observe::Gradient(0xffffff00, 0xff00ffff), point1_, point2_);
    canvas.setColor(points);
    canvas.roundedRectangle(0, 0, width(), height(), 18.0f);

    canvas.setColor(0xff000000);
    observe::Font font(20, resources::fonts::Lato_Regular_ttf);
    canvas.text("Linear Points", font, observe::Font::kCenter, 0, 0, width(), height());

    canvas.setColor(mouse_down_ ? 0xaaffffff : 0x66ffffff);
    if (active_point_ == kFrom)
      canvas.circle(point1_.x - kDragRadius, point1_.y - kDragRadius, 2.0f * kDragRadius);

    else if (active_point_ == kTo)
      canvas.circle(point2_.x - kDragRadius, point2_.y - kDragRadius, 2.0f * kDragRadius);

    canvas.setColor(0xff000000);
    canvas.circle(point1_.x - kDotRadius, point1_.y - kDotRadius, 2.0f * kDotRadius);
    canvas.circle(point2_.x - kDotRadius, point2_.y - kDotRadius, 2.0f * kDotRadius);
  }

  void setActivePoint(ActivePoint active_point)
  {
    if (active_point == active_point_)
      return;

    active_point_ = active_point;
    redraw();
  }

  void mouseMove(const observe::MouseEvent &e) override
  {
    observe::Point point = e.position;
    observe::Point delta_from = point - point1_;
    observe::Point delta_to = point - point2_;

    if (delta_from.squareMagnitude() < kDragRadius * kDragRadius &&
        delta_from.squareMagnitude() < delta_to.squareMagnitude())
      setActivePoint(kFrom);
    else if (delta_to.squareMagnitude() < kDragRadius * kDragRadius)
      setActivePoint(kTo);
    else
      setActivePoint(kNone);
  }

  void mouseDown(const observe::MouseEvent &e) override
  {
    if (active_point_ == kNone)
      return;

    mouse_down_ = true;
    redraw();
  }

  void mouseUp(const observe::MouseEvent &e) override
  {
    if (active_point_ == kNone)
      return;

    mouse_down_ = false;
    redraw();
  }

  void mouseDrag(const observe::MouseEvent &e) override
  {
    if (active_point_ == kNone)
      return;

    if (active_point_ == kFrom)
      point1_ = localBounds().clampPoint(e.position);
    else if (active_point_ == kTo)
      point2_ = localBounds().clampPoint(e.position);
    redraw();
  }

  void setGradient(const observe::Gradient &gradient)
  {
    gradient_ = gradient;
    redraw();
  }

  void setPoints(const observe::Point &point1, const observe::Point &point2)
  {
    point1_ = point1;
    point2_ = point2;
    redraw();
  }

protected:
  ActivePoint active_point_ = kNone;
  bool mouse_down_ = false;
  observe::Point point1_;
  observe::Point point2_;
  observe::Gradient gradient_;
};

class LinearPointsFrame : public PointsFrame
{
public:
  void draw(observe::Canvas &canvas) override
  {
    observe::Brush points = observe::Brush::linear(gradient_, point1_, point2_);
    canvas.setColor(points);
    canvas.roundedRectangle(0, 0, width(), height(), 18.0f);

    canvas.setColor(0xff000000);
    observe::Font font(20, resources::fonts::Lato_Regular_ttf);
    canvas.text("Linear Gradient", font, observe::Font::kCenter, 0, 0, width(), height());

    canvas.setColor(mouse_down_ ? 0xaaffffff : 0x66ffffff);
    if (active_point_ == kFrom)
      canvas.circle(point1_.x - kDragRadius, point1_.y - kDragRadius, 2.0f * kDragRadius);

    else if (active_point_ == kTo)
      canvas.circle(point2_.x - kDragRadius, point2_.y - kDragRadius, 2.0f * kDragRadius);

    canvas.setColor(0xff000000);
    canvas.circle(point1_.x - kDotRadius, point1_.y - kDotRadius, 2.0f * kDotRadius);
    canvas.circle(point2_.x - kDotRadius, point2_.y - kDotRadius, 2.0f * kDotRadius);
  }
};

class RadialPointsFrame : public PointsFrame
{
public:
  void draw(observe::Canvas &canvas) override
  {
    auto radius = (point2_ - point1_).length();
    observe::Brush radial = observe::Brush::radial(gradient_, point1_, radius);
    canvas.setColor(radial);
    canvas.roundedRectangle(0, 0, width(), height(), 18.0f);

    canvas.setColor(0xff000000);
    observe::Font font(20, resources::fonts::Lato_Regular_ttf);
    canvas.text("Radial Gradient", font, observe::Font::kCenter, 0, 0, width(), height());

    canvas.setColor(mouse_down_ ? 0xaaffffff : 0x66ffffff);
    if (active_point_ == kFrom)
      canvas.circle(point1_.x - kDragRadius, point1_.y - kDragRadius, 2.0f * kDragRadius);

    else if (active_point_ == kTo)
      canvas.circle(point2_.x - kDragRadius, point2_.y - kDragRadius, 2.0f * kDragRadius);

    canvas.setColor(0xff000000);
    canvas.circle(point1_.x - kDotRadius, point1_.y - kDotRadius, 2.0f * kDotRadius);
    canvas.circle(point2_.x - kDotRadius, point2_.y - kDotRadius, 2.0f * kDotRadius);
  }
};

int runExample()
{
  observe::ApplicationWindow app;

  app.layout().setFlex(true);
  app.layout().setFlexGap(8);
  app.layout().setPadding(8);

  observe::Frame gradients;
  gradients.layout().setFlex(true);
  gradients.layout().setFlexRows(false);
  gradients.layout().setFlexGap(8);
  gradients.layout().setFlexGrow(1.0f);
  app.addChild(gradients);

  app.onDraw() = [&app](observe::Canvas &canvas)
  {
    canvas.setColor(0xff222222);
    canvas.fill(0, 0, app.width(), app.height());
  };

  observe::Gradient gradient(0xffffff00, 0xff00aaff);

  std::unique_ptr<PointsFrame> linear_points_frame = std::make_unique<LinearPointsFrame>();
  gradients.addChild(linear_points_frame.get());
  linear_points_frame->layout().setFlexGrow(1.0f);
  linear_points_frame->setGradient(gradient);

  std::unique_ptr<PointsFrame> radial_points_frame = std::make_unique<RadialPointsFrame>();
  gradients.addChild(radial_points_frame.get());
  radial_points_frame->layout().setFlexGrow(1.0f);
  radial_points_frame->setGradient(gradient);

  observe::Font font(20, resources::fonts::Lato_Regular_ttf);
  observe::Frame controls;
  controls.layout().setFlexGrow(0.15f);
  controls.layout().setFlex(true);
  controls.layout().setFlexRows(false);
  controls.layout().setFlexGap(8);

  std::string pattern = "Single";
  observe::UiButton pattern_button("Pattern: " + pattern);
  pattern_button.setFont(font);
  pattern_button.layout().setFlexGrow(1.0f);
  controls.addChild(pattern_button);

  pattern_button.onToggle() = [&](observe::Button *button, bool on)
  {
    if (pattern == "Single")
      pattern = "Repeat";
    else if (pattern == "Repeat")
      pattern = "Reflect";
    else
      pattern = "Single";

    setPattern(gradient, pattern);
    linear_points_frame->setGradient(gradient);
    radial_points_frame->setGradient(gradient);
    pattern_button.setText("Pattern: " + pattern);
  };

  std::map<std::string, observe::Gradient> gradients_map = {
      {"Two Color", observe::Gradient(0xffffff00, 0xff00aaff)},
      {"Rainbow", observe::Gradient(0xffff0000, 0xffffff00, 0xff00ff00, 0xff00ffff, 0xff0000ff,
                                   0xffff00ff, 0xffff0000)},
      {"Rainbow (OkLab)", observe::Gradient::fromSampleFunction(100, sampleOkLab)},
      {"Viridis", observe::Gradient::kViridis}};

  observe::UiButton color_button("Gradient: Two Color");
  color_button.setFont(font);
  color_button.layout().setFlexGrow(1.0f);
  controls.addChild(color_button);

  app.addChild(controls);
  auto gradient_it = gradients_map.find("Two Color");
  color_button.onToggle() = [&](observe::Button *button, bool on)
  {
    gradient_it = std::next(gradient_it);
    if (gradient_it == gradients_map.end())
      gradient_it = gradients_map.begin();

    gradient = gradient_it->second;
    setPattern(gradient, pattern);
    linear_points_frame->setGradient(gradient);
    radial_points_frame->setGradient(gradient);
    color_button.setText("Gradient: " + gradient_it->first);
  };

  app.setTitle("Observe Gradient Example");
  app.show(80_vmin, 60_vmin);
  app.runEventLoop();
  return 0;
}
