#include "embedded/example_fonts.h"

#include <observe/app.h>
#include <modules/observe_widgets/button.h>

using namespace observe::dimension;

class SubWindow : public observe::ApplicationWindow
{
public:
  void draw(observe::Canvas &canvas) override
  {
    canvas.setColor(0xff222026);
    canvas.fill(0, 0, width(), height());

    canvas.setColor(0xffaa88ff);
    int radius = std::min(width(), height()) / 4;
    canvas.circle(width() / 2 - radius, height() / 2 - radius, 2 * radius);
  }
};

class ExampleEditor : public observe::ApplicationWindow
{
public:
  static constexpr int kNumFrames = 10;

  ExampleEditor()
  {
    setFlexLayout(true);
    layout().setPadding(50_px);

    addChild(&button_);
    button_.setText("Open Window");
    button_.layout().setHeight(100_vh);
    button_.layout().setWidth(100_vw);
    button_.setFont(observe::Font(24, resources::fonts::Lato_Regular_ttf));

    sub_window_.onShow() += [this]
    { button_.setText("Close Window"); };
    sub_window_.onHide() += [this]
    { button_.setText("Open Window"); };

    button_.onToggle() += [this](observe::Button *button, bool is_toggled)
    {
      if (sub_window_.isShowing())
        sub_window_.hide();
      else
        sub_window_.show(10_vw, 10_vh, 400_px, 300_px);
    };

    onHide() += [this]
    {
      if (sub_window_.isShowing())
        sub_window_.hide();
    };
  }

  void draw(observe::Canvas &canvas) override
  {
    canvas.setColor(0xff222026);
    canvas.fill(0, 0, width(), height());
  }

private:
  observe::UiButton button_;
  observe::Font font_{};
  SubWindow sub_window_;
};

int runExample()
{
  ExampleEditor editor;
  editor.show(500_px, 200_px);
  editor.runEventLoop();

  return 0;
}
