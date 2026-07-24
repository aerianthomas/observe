#include "embedded/example_fonts.h"
#include "embedded/example_shaders.h"

#include <complex>
#include <observe/app.h>
#include <observe/widgets.h>

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

class PostEffectSelector : public observe::Frame
{
public:
  enum PostEffect
  {
    kNone,
    kGrayScale,
    kSepia,
    kGlitch,
    kBlur,
    kNumOptions
  };

  PostEffectSelector()
  {
    static const std::string option_names[kNumOptions] = {"None", "Gray Scale", "Sepia", "Glitch", "Blur"};

    setFlexLayout(true);
    layout().setPadding(3_vmin);
    layout().setFlexGap(3_vmin);
    for (int i = 0; i < kNumOptions; ++i)
    {
      options_[i].layout().setFlexGrow(1.0f);
      options_[i].setText(option_names[i]);

      addChild(&options_[i]);
      options_[i].onToggle() = [this](observe::Button *button, bool on)
      {
        for (int i = 0; i < kNumOptions; ++i)
        {
          if (&options_[i] == button && on_effect_change_)
            on_effect_change_(static_cast<PostEffect>(i));
        }
      };
    }
  }

  void resized() override
  {
    observe::Font font(height() * 0.05f, resources::fonts::Lato_Regular_ttf);

    for (auto &button : options_)
      button.setFont(font);
  }

  void setCallback(std::function<void(PostEffect)> on_effect_change)
  {
    on_effect_change_ = std::move(on_effect_change);
  }

private:
  observe::UiButton options_[kNumOptions]{};
  std::function<void(PostEffect)> on_effect_change_;
};

class ExampleEditor : public observe::ApplicationWindow
{
public:
  static constexpr float kBackdropWidth = 300.0f;

  ExampleEditor()
  {
    setReceiveChildMouseEvents(true);
    shapes_.onDraw() = [this](observe::Canvas &canvas)
    {
      float width = shapes_.width();
      float height = shapes_.height();
      float min = std::min(width, height);

      canvas.setColor(0xff222233);
      canvas.fill(0.0f, 0.0f, width, height);
      canvas.setColor(0xffaa88ff);
      drawRing(canvas, width, height, min * 0.3f, min * 0.2f, 8, canvas.time() * 0.1f);
      shapes_.redraw();
    };

    gray_scale_ = std::make_unique<observe::ShaderPostEffect>(resources::shaders::vs_custom,
                                                             resources::shaders::fs_gray_scale);

    sepia_ = std::make_unique<observe::ShaderPostEffect>(resources::shaders::vs_custom,
                                                        resources::shaders::fs_sepia);

    glitch_ = std::make_unique<observe::ShaderPostEffect>(resources::shaders::vs_custom,
                                                         resources::shaders::fs_glitch);

    blur_ = std::make_unique<observe::BlurPostEffect>();
    blur_->setBlurRadius(40.0f);

    addChild(shapes_);
    addChild(selector_);
    addChild(blur_glass_);

    selector_.setCallback([this](PostEffectSelector::PostEffect effect)
                          {
      if (effect == PostEffectSelector::kNone)
        shapes_.setPostEffect(nullptr);
      else if (effect == PostEffectSelector::kGrayScale)
        shapes_.setPostEffect(gray_scale_.get());
      else if (effect == PostEffectSelector::kSepia)
        shapes_.setPostEffect(sepia_.get());
      else if (effect == PostEffectSelector::kGlitch)
        shapes_.setPostEffect(glitch_.get());
      else if (effect == PostEffectSelector::kBlur)
        shapes_.setPostEffect(blur_.get()); });

    blur_glass_.setBackdropEffect(&blur_backdrop_);
    blur_backdrop_.setBlurRadius(40.0f);

    blur_glass_.onDraw() = [&](observe::Canvas &canvas)
    {
      canvas.setColor(0x22ffffff);
      canvas.fill(0, 0, blur_glass_.width(), blur_glass_.height());

      canvas.setBlendMode(observe::BlendMode::MaskRemove);
      canvas.setColor(0xffffffff);
      canvas.fill(0, 0, blur_glass_.width(), blur_glass_.height());

      canvas.setBlendMode(observe::BlendMode::MaskAdd);
      canvas.setColor(0xffffffff);
      canvas.roundedRectangle(0, 0, blur_glass_.width(), blur_glass_.height(),
                              0.5f * std::min(blur_glass_.width(), blur_glass_.height()));

      canvas.setBlendMode(observe::BlendMode::Alpha);
      canvas.setColor(observe::Brush::vertical(0x44ffffff, 0x44000000));
      canvas.roundedRectangleBorder(0, 0, blur_glass_.width(), blur_glass_.height(),
                                    0.5f * std::min(blur_glass_.width(), blur_glass_.height()), 2.0f);
      canvas.setColor(0xffffffff);
      observe::Font font(16.0f, resources::fonts::Lato_Regular_ttf);
      canvas.text("Drag me", font, observe::Font::Justification::kCenter, 0, 0, blur_glass_.width(),
                  blur_glass_.height());
    };

    onMouseDrag() = [&](const observe::MouseEvent &e)
    {
      if (e.event_frame != &blur_glass_)
        return;

      float x = e.windowPosition().x - kBackdropWidth * 0.5f;
      float y = e.windowPosition().y - kBackdropWidth * 0.5f;
      blur_glass_.setBounds(x, y, kBackdropWidth, kBackdropWidth);
    };

    blur_glass_.onMouseEnter() = [&](const observe::MouseEvent &e)
    {
      setCursorStyle(observe::MouseCursor::MultiDirectionalResize);
    };

    blur_glass_.onMouseExit() = [&](const observe::MouseEvent &e)
    {
      setCursorStyle(observe::MouseCursor::Arrow);
    };
  }

  void draw(observe::Canvas &canvas) override
  {
    canvas.setColor(0xff222233);
    canvas.fill(0, 0, width(), height());
  }

  void resized() override
  {
    float center = width() / 2.0f;
    float shapes_width = std::min(center, height());
    shapes_.setBounds((center - shapes_width) / 2.0f, (height() - shapes_width) / 2.0f,
                      shapes_width, shapes_width);
    selector_.setBounds(center, 0.0f, width() - center, height());
    blur_glass_.setBounds(shapes_.x() + (shapes_.width() - kBackdropWidth) * 0.5f,
                          shapes_.y() + (shapes_.height() - kBackdropWidth) * 0.5f, kBackdropWidth,
                          kBackdropWidth);
  }

private:
  Frame blur_glass_;
  observe::BlurPostEffect blur_backdrop_;

  PostEffectSelector selector_;
  Frame shapes_;
  std::unique_ptr<observe::ShaderPostEffect> gray_scale_;
  std::unique_ptr<observe::ShaderPostEffect> sepia_;
  std::unique_ptr<observe::ShaderPostEffect> glitch_;
  std::unique_ptr<observe::BlurPostEffect> blur_;
};

int runExample()
{
  ExampleEditor editor;
  editor.showMaximized();
  editor.runEventLoop();

  return 0;
}
