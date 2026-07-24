#pragma once

#include "examples_frame.h"

#include <observe/app.h>
#include <modules/observe_graphics/palette.h>
#include <modules/observe_ui/popup_menu.h>
#include <modules/observe_ui/undo_history.h>
#include <modules/observe_utils/dimension.h>
#include <modules/observe_widgets/palette_editor.h>
#include <modules/observe_widgets/shader_editor.h>

class DebugInfo;

class Overlay : public observe::Frame
{
public:
  Overlay();

  void resized() override;
  void draw(observe::Canvas &canvas) override;
  observe::Bounds bodyBounds() const;
  float bodyRounding();

  void mouseDown(const observe::MouseEvent &e) override
  {
    animation_.target(false);
    redraw();
  }
  void visibilityChanged() override { animation_.target(isVisible()); }
  auto &onAnimate() { return on_animate_; }

private:
  observe::Animation<float> animation_;
  observe::CallbackList<void(float)> on_animate_;
};

class PaletteColorWindow : public observe::ApplicationWindow
{
public:
  explicit PaletteColorWindow(observe::Palette *palette) : editor_(palette)
  {
    addChild(&editor_);
    editor_.layout().setMargin(0);
  }

private:
  observe::PaletteColorEditor editor_;
};

class PaletteValueWindow : public observe::ApplicationWindow
{
public:
  explicit PaletteValueWindow(observe::Palette *palette) : editor_(palette)
  {
    addChild(&editor_);
    editor_.layout().setMargin(0);
  }

private:
  observe::PaletteValueEditor editor_;
};

class Showcase : public observe::Frame,
                 public observe::UndoHistory
{
public:
  Showcase();
  ~Showcase() override;

  void resized() override;
  void draw(observe::Canvas &canvas) override;

  void toggleDebug();
  bool keyPress(const observe::KeyEvent &key) override;

private:
  std::unique_ptr<observe::ShaderPostEffect> overlay_zoom_;
  std::unique_ptr<ExamplesFrame> examples_;
  std::unique_ptr<DebugInfo> debug_info_;

  observe::Palette palette_;
  PaletteColorWindow palette_color_window_;
  PaletteValueWindow palette_value_window_;
  Overlay overlay_;

  OBSERVE_LEAK_CHECKER(Showcase)
};
