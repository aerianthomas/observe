#include <observe/app.h>
#include <modules/observe_ui/popup_menu.h>

class ExampleEditor : public observe::ApplicationWindow
{
public:
  void draw(observe::Canvas &canvas) override
  {
    static constexpr float kRadius = 30.0f;
    canvas.setColor(0xff000000);
    canvas.fill(0, 0, width(), height());
    if (down_)
      canvas.setColor(0xff00ffff);
    else
      canvas.setColor(0xffffffff);
    canvas.circle(x_ - kRadius, y_ - kRadius, 2.0f * kRadius);
  }

  void setPosition(const observe::Point &point)
  {
    x_ = point.x;
    y_ = point.y;
    redraw();
  }

  void setDown(bool down)
  {
    down_ = down;
    redraw();
  }

  void mouseMove(const observe::MouseEvent &e) override { setPosition(e.position); }
  void mouseDrag(const observe::MouseEvent &e) override { setPosition(e.position); }
  void mouseExit(const observe::MouseEvent &e) override { setPosition({-100, -100}); }
  void mouseDown(const observe::MouseEvent &e) override
  {
    if (e.isMiddleButton())
    {
      setMouseRelativeMode(true);
      setCursorVisible(false);
    }

    if (!e.shouldTriggerPopup())
    {
      setDown(true);
      return;
    }
    observe::PopupMenu menu;
    menu.addOption(1, "Option 1");
    menu.addOption(2, "Option 2");
    menu.show(this, {x_, y_});
  }
  void mouseUp(const observe::MouseEvent &e) override
  {
    if (e.isMiddleButton())
    {
      setCursorVisible(true);
      setMouseRelativeMode(false);
    }
    setDown(false);
  }

private:
  bool down_ = false;
  float x_ = -100.0f;
  float y_ = -100.0f;
};

int runExample()
{
  ExampleEditor editor;
  editor.show(observe::Dimension::logicalPixels(800), observe::Dimension::logicalPixels(600));
  editor.runEventLoop();

  return 0;
}
