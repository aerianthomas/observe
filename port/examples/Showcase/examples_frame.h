#pragma once

#include <modules/observe_widgets/bar_list.h>
#include <modules/observe_widgets/button.h>
#include <modules/observe_widgets/shader_quad.h>
#include <modules/observe_widgets/text_editor.h>

class DragDropExample;
class AnimatedLines;

class TitleBar : public observe::Frame
{
public:
  explicit TitleBar(const std::string &title) : title_(title) { setIgnoresMouseEvents(true, true); }

  void draw(observe::Canvas &canvas) override;

private:
  std::string title_;
};

class ExampleSection : public observe::Frame
{
public:
  ExampleSection(const std::string &title, Frame *example);

private:
  std::string title_;
  TitleBar title_bar_;
  observe::Frame *example_;
};

class ExamplesFrame : public observe::ScrollableFrame
{
public:
  static constexpr int kNumLines = 2;
  static constexpr int kNumBars = 21;

  ExamplesFrame();
  ~ExamplesFrame() override;

  void setupBars();
  void setupButtons();
  void setupShapes();
  void setupTextEditors();

  auto &onScreenshot() { return on_screenshot_; }
  auto &onShowOverlay() { return on_show_overlay_; }
  auto &onToggleDebug() { return on_toggle_debug_; }

private:
  std::vector<std::unique_ptr<ExampleSection>> sections_;

  observe::Frame button_container_;
  observe::Frame text_editor_container_;
  observe::Frame shader_container_;
  observe::Frame image_container_;

  observe::CallbackList<void(const std::string &file_path)> on_screenshot_;
  observe::CallbackList<void()> on_show_overlay_;
  observe::CallbackList<void()> on_toggle_debug_;

  std::unique_ptr<DragDropExample> drag_drop_;
  std::unique_ptr<observe::BarList> bar_list_;
  std::unique_ptr<observe::ShaderQuad> shader_quad_;
  std::unique_ptr<observe::ToggleIconButton> icon_button_;
  std::unique_ptr<observe::ToggleTextButton> text_button_;
  std::unique_ptr<observe::UiButton> ui_button_;
  std::unique_ptr<observe::UiButton> action_button_;
  std::unique_ptr<observe::TextEditor> text_editor_;
  std::unique_ptr<observe::TextEditor> left_text_editor_;
  std::unique_ptr<observe::TextEditor> center_editor_;
  std::unique_ptr<observe::TextEditor> right_text_editor_;
  std::unique_ptr<observe::Frame> shapes_;
  std::unique_ptr<AnimatedLines> animated_lines_;
};
