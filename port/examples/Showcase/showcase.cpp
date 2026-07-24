#include "showcase.h"

#include "embedded/example_fonts.h"
#include "embedded/example_shaders.h"

#include <observe/app.h>
#include <observe/graphics.h>
#include <observe/widgets.h>

using namespace observe::dimension;

OBSERVE_THEME_COLOR(BackgroundColor, 0xff33393f);
OBSERVE_THEME_COLOR(OverlayBody, 0xff212529);
OBSERVE_THEME_COLOR(OverlayBorder, 0x66ffffff);

OBSERVE_THEME_VALUE(BloomSize, 25.0f);
OBSERVE_THEME_VALUE(BloomIntensity, 3.0f);
OBSERVE_THEME_VALUE(BlurSize, 80.0f);
OBSERVE_THEME_VALUE(OverlayRounding, 25.0f);

class DebugInfo : public observe::Frame
{
public:
  DebugInfo() { setIgnoresMouseEvents(true, true); }

  void draw(observe::Canvas &canvas) override
  {
    canvas.setColor(0x88000000);
    canvas.fill(0, 0, width(), height());

    canvas.setColor(0xffffffff);

    std::vector<std::string> info = canvas.debugInfo();
    int line_height = height() / info.size();
    int text_height = line_height * 0.65f;

    const observe::Font font(text_height, resources::fonts::Lato_Regular_ttf);
    for (int i = 0; i < info.size(); ++i)
      canvas.text(info[i], font, observe::Font::kLeft, line_height, line_height * i, width(), line_height);
    redraw();
  }
};

Overlay::Overlay() : animation_(observe::Animation<float>::kRegularTime, observe::Animation<float>::kLinear,
                                observe::Animation<float>::kLinear)
{
  animation_.setAnimationTime(160.0f);
  animation_.setTargetValue(1.0f);
}

void Overlay::resized() {}

void Overlay::draw(observe::Canvas &canvas)
{
  float overlay_amount = animation_.update();
  if (!animation_.isTargeting() && overlay_amount == 0.0f)
    setVisible(false);

  observe::Bounds body = bodyBounds();
  float rounding = bodyRounding();
  canvas.setColor(OverlayBody);
  canvas.roundedRectangle(body.x(), body.y(), body.width(), body.height(), rounding);

  canvas.setColor(OverlayBorder);
  canvas.roundedRectangleBorder(body.x(), body.y(), body.width(), body.height(), rounding, 1.0f);

  on_animate_.callback(overlay_amount);

  if (animation_.isAnimating())
    redraw();
}

observe::Bounds Overlay::bodyBounds() const
{
  float x_border = width() / 4.0f;
  float y_border = height() / 4.0f;

  return {x_border, y_border, width() - 2 * x_border, height() - 2 * y_border};
}

float Overlay::bodyRounding()
{
  return paletteValue(OverlayRounding);
}

Showcase::Showcase() : palette_color_window_(&palette_), palette_value_window_(&palette_)
{
  setIgnoresMouseEvents(true, true);
  setAcceptsKeystrokes(true);

  palette_.initWithDefaults();
  setPalette(&palette_);

  examples_ = std::make_unique<ExamplesFrame>();
  examples_->onShowOverlay() = [this]
  { overlay_.setVisible(true); };
  examples_->onToggleDebug() = [this]
  { toggleDebug(); };

  examples_->onScreenshot() = [this](const std::string &file_path)
  {
    observe::ApplicationEditor *parent = findParent<observe::ApplicationEditor>();
    if (parent)
      parent->takeScreenshot().save(file_path);
  };

  addChild(examples_.get());
  overlay_zoom_ = std::make_unique<observe::ShaderPostEffect>(resources::shaders::vs_overlay,
                                                             resources::shaders::fs_overlay);
  overlay_.setPostEffect(overlay_zoom_.get());
  addChild(&overlay_, false);
  overlay_.onAnimate() = [this](float overlay_amount)
  {
    static constexpr float kMaxZoom = 0.075f;
    examples_->setBlurRadius(paletteValue(BlurSize) * overlay_amount);
    overlay_zoom_->setUniformValue("u_zoom", kMaxZoom * (1.0f - overlay_amount) + 1.0f);
    overlay_zoom_->setUniformValue("u_alpha", overlay_amount * overlay_amount);
  };

  debug_info_ = std::make_unique<DebugInfo>();
  addChild(debug_info_.get());
  debug_info_->setOnTop(true);
  debug_info_->setVisible(false);
}

Showcase::~Showcase() = default;

void Showcase::resized()
{
  int w = width();
  int h = height();
  int main_width = w;

  debug_info_->setBounds(0, 0, main_width, h);
  examples_->setBounds(0, 0, main_width, h);
  overlay_.setBounds(examples_->bounds());
}

void Showcase::draw(observe::Canvas &canvas)
{
  canvas.setPalette(palette());
}

void Showcase::toggleDebug()
{
  debug_info_->setVisible(!debug_info_->isVisible());
}

bool Showcase::keyPress(const observe::KeyEvent &key)
{
  bool modifier = key.isMainModifier();
  if (key.keyCode() == observe::KeyCode::D && modifier && key.isShiftDown())
    toggleDebug();
  else if (key.keyCode() == observe::KeyCode::Z && modifier)
    undo();
  else if (key.keyCode() == observe::KeyCode::Y && modifier)
    redo();
  else if (key.keyCode() == observe::KeyCode::Number1 && modifier)
    palette_color_window_.show(300_px, 800_px);
  else
    return false;

  return true;
}

int runExample()
{
  observe::ApplicationWindow editor;

  editor.onDraw() = [&editor](observe::Canvas &canvas)
  {
    canvas.setColor(BackgroundColor);
    canvas.fill(0, 0, editor.width(), editor.height());
  };

  std::unique_ptr<Showcase> showcase = std::make_unique<Showcase>();
  editor.addChild(showcase.get());
  editor.layout().setFlex(true);
  editor.layout().setFlexItemAlignment(observe::Layout::ItemAlignment::Center);
  showcase->layout().setWidth(observe::Dimension::min(1000_px, 100_vw));
  showcase->layout().setHeight(100_vh);

  editor.setTitle("Observe Showcase");
  if (observe::isMobileDevice())
    editor.showMaximized();
  else
    editor.show(100_vmin, 70_vmin);
  editor.runEventLoop();
  return 0;
}
