#include <observe/app.h>

using namespace observe::dimension;

class ExampleEditor : public observe::ApplicationWindow
{
public:
  static constexpr int kNumFrames = 10;

  ExampleEditor()
  {
    setFlexLayout(true);
    layout().setPadding(10_px);
    layout().setFlexGap(10_px);
    layout().setFlexWrap(true);
    layout().setFlexReverseDirection(true);
    layout().setFlexWrapReverse(true);

    int i = 0;
    for (Frame &frame : frames_)
    {
      addChild(&frame);
      frame.layout().setHeight(100);
      frame.layout().setWidth(100 + i * 10);
      frame.layout().setFlexGrow(1.0f);

      frame.onDraw() = [&frame](observe::Canvas &canvas)
      {
        canvas.setColor(0xff888888);
        canvas.roundedRectangle(0, 0, frame.width(), frame.height(), 16);
      };
      ++i;
    }
  }

  void draw(observe::Canvas &canvas) override
  {
    canvas.setColor(0xff222026);
    canvas.fill(0, 0, width(), height());
  }

private:
  Frame frames_[kNumFrames];
};

int runExample()
{
  ExampleEditor editor;
  editor.show(observe::Dimension::logicalPixels(800), observe::Dimension::logicalPixels(600));
  editor.runEventLoop();

  return 0;
}
