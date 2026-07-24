#include "examples_frame.h"

#include "embedded/example_fonts.h"
#include "embedded/example_icons.h"
#include "embedded/example_images.h"
#include "embedded/example_shaders.h"

#include <filesystem>
#include <modules/observe_graphics/theme.h>
#include <modules/observe_ui/popup_menu.h>
#include <modules/observe_utils/file_system.h>
#include <modules/observe_widgets/graph_line.h>
#include <modules/observe_windowing/windowing.h>

using namespace observe::dimension;

inline float quickSin1(float phase)
{
  phase = 0.5f - phase;
  return phase * (8.0f - 16.0f * fabsf(phase));
}

inline float sin1(float phase)
{
  float approx = quickSin1(phase - floorf(phase));
  return approx * (0.776f + 0.224f * fabsf(approx));
}

static constexpr float kHalfPi = 3.14159265358979323846f * 0.5f;

OBSERVE_THEME_COLOR(TextColor, 0xffffffff);
OBSERVE_THEME_COLOR(ShapeColor, 0xffaaff88);
OBSERVE_THEME_COLOR(LabelColor, 0x44212529);
OBSERVE_THEME_COLOR(DarkBackgroundColor, 0xff212529);
OBSERVE_THEME_COLOR(OverlayShadowColor, 0xbb000000);
OBSERVE_THEME_COLOR(ShadowColor, 0x88000000);

class AnimatedLines : public observe::Frame
{
public:
  static constexpr int kNumLines = 2;
  static constexpr int kNumPoints = 400;

  AnimatedLines()
  {
    for (auto &graph_line : graph_lines_)
    {
      graph_line = std::make_unique<observe::GraphLine>(kNumPoints);
      addChild(graph_line.get());
    }
  }

  void resized() override
  {
    int line_offset = height() / kNumLines;
    for (int i = 0; i < kNumLines; ++i)
    {
      graph_lines_[i]->setBounds(0, line_offset * i, width(), line_offset);
      graph_lines_[i]->setFilled(true);
    }
  }

  void draw(observe::Canvas &canvas) override
  {
    double render_time = canvas.time();
    for (int r = 0; r < kNumLines; ++r)
    {
      float position = 0.0f;
      for (int i = 0; i < kNumPoints; ++i)
      {
        float t = i / (kNumPoints - 1.0f);
        float delta = std::min(t, 1.0f - t);
        position += 0.1f * delta * delta + 0.003f;
        double phase = (render_time + r) * 0.5;
        float y = sin1(phase + position) * 0.4f + 0.5f;
        graph_lines_[r]->set(i, y);
      }
    }

    redraw();
  }

private:
  std::unique_ptr<observe::GraphLine> graph_lines_[kNumLines];
};

class DragDropSource : public observe::Frame
{
public:
  void draw(observe::Canvas &canvas) override
  {
    canvas.setColor(DarkBackgroundColor);
    canvas.roundedRectangle(0, 0, width(), height(), height() / 16);

    canvas.setColor(TextColor);

    const observe::Font font(height() / 4, resources::fonts::Lato_Regular_ttf);
    if (dragging_)
      canvas.text("Dragging source file", font, observe::Font::kCenter, 0, 0, width(), height());
    else
      canvas.text("Drag source", font, observe::Font::kCenter, 0, 0, width(), height());
  }

  bool isDragDropSource() override { return true; }

  std::string startDragDropSource() override
  {
    redraw();
    dragging_ = true;
    source_file_ = observe::createTemporaryFile("txt");
    observe::replaceFileWithText(source_file_, "Example drag and drop source file.");
    return source_file_.string();
  }

  void cleanupDragDropSource() override
  {
    redraw();
    dragging_ = false;
    if (std::filesystem::exists(source_file_))
      std::filesystem::remove(source_file_);
  }

private:
  bool dragging_ = false;
  observe::File source_file_;
};

class DragDropTarget : public observe::Frame
{
public:
  void draw(observe::Canvas &canvas) override
  {
    canvas.setColor(DarkBackgroundColor);
    canvas.roundedRectangle(0, 0, width(), height(), height() / 16);

    canvas.setColor(TextColor);

    const observe::Font font(height() / 4, resources::fonts::Lato_Regular_ttf);
    if (dragging_)
      canvas.text("Dragging " + filename_, font, observe::Font::kCenter, 0, 0, width(), height());
    else if (dropped_)
      canvas.text("Dropped " + filename_, font, observe::Font::kCenter, 0, 0, width(), height());
    else
      canvas.text("Drag destination", font, observe::Font::kCenter, 0, 0, width(), height());
  }

  bool receivesDragDropFiles() override { return true; }
  std::string dragDropFileExtensionRegex() override { return ".*"; }

  void dragFilesEnter(const std::vector<std::string> &paths) override
  {
    dragging_ = true;
    dropped_ = false;
    filename_ = observe::fileName(paths[0]);
    redraw();
  }

  void dragFilesExit() override
  {
    dragging_ = false;
    redraw();
  }

  void dropFiles(const std::vector<std::string> &paths) override
  {
    dragging_ = false;
    dropped_ = true;
    redraw();
  }

private:
  std::string filename_;
  bool dragging_ = false;
  bool dropped_ = false;
};

class DragDropExample : public observe::Frame
{
public:
  DragDropExample()
  {
    addChild(&source_);
    addChild(&target_);
  }

  void resized() override
  {
    int padding = height() / 16;
    int h = (height() - padding) / 2;
    source_.setBounds(0, 0, width(), h);
    target_.setBounds(0, height() - h, width(), h);
  }

private:
  DragDropSource source_;
  DragDropTarget target_;
};

void TitleBar::draw(observe::Canvas &canvas)
{
  canvas.setColor(DarkBackgroundColor);
  canvas.rectangle(0, 0, width(), height());

  canvas.setColor(TextColor);
  const observe::Font font(height() / 2, resources::fonts::Lato_Regular_ttf);
  canvas.text(title_, font, observe::Font::kCenter, 0, 0, width(), height());
}

ExampleSection::ExampleSection(const std::string &title, observe::Frame *example) : title_bar_(title), example_(example)
{
  setIgnoresMouseEvents(true, true);

  addChild(&title_bar_);
  addChild(example_);

  layout().setFlex(true);
  layout().setFlexGrow(1.0f);
  layout().setPaddingTop(8_px);
  layout().setFlexGap(8_px);
  layout().setWidth(400_px);
  layout().setHeight(220_px);
  title_bar_.layout().setWidth(100_vw);
  title_bar_.layout().setHeight(32_px);
  example_->layout().setWidth(100_vw);
  example_->layout().setFlexGrow(1.0f);
}

ExamplesFrame::ExamplesFrame()
{
  animated_lines_ = std::make_unique<AnimatedLines>();
  sections_.push_back(std::make_unique<ExampleSection>("Line Rendering", animated_lines_.get()));

  setupButtons();
  sections_.push_back(std::make_unique<ExampleSection>("Buttons", &button_container_));

  setupShapes();
  sections_.push_back(std::make_unique<ExampleSection>("Shapes", shapes_.get()));

  setupTextEditors();
  sections_.push_back(std::make_unique<ExampleSection>("Text Editing", &text_editor_container_));

  image_container_.setMasked(true);
  image_container_.onDraw() = [this](observe::Canvas &canvas)
  {
    canvas.setColor(0xffffffff);
    int offset = (image_container_.width() - image_container_.height()) / 2;
    canvas.image(resources::images::test_png, offset, 0, image_container_.height(),
                 image_container_.height());
    canvas.setBlendMode(observe::BlendMode::Mult);
    canvas.squircle(offset, 0, image_container_.height());
  };

  sections_.push_back(std::make_unique<ExampleSection>("Images", &image_container_));

  shader_quad_ = std::make_unique<observe::ShaderQuad>(resources::shaders::vs_shader_quad,
                                                      resources::shaders::fs_shader_quad,
                                                      observe::BlendMode::Alpha);
  shader_container_.layout().setFlex(true);
  shader_container_.layout().setFlexItemAlignment(observe::Layout::ItemAlignment::Center);
  shader_container_.addChild(shader_quad_.get());
  shader_quad_->layout().setWidth(100_vmin);
  shader_quad_->layout().setHeight(100_vmin);
  sections_.push_back(std::make_unique<ExampleSection>("Shaders", &shader_container_));

  setupBars();
  sections_.push_back(std::make_unique<ExampleSection>("Bars", bar_list_.get()));

  drag_drop_ = std::make_unique<DragDropExample>();
  sections_.push_back(std::make_unique<ExampleSection>("File Drag and Drop", drag_drop_.get()));

  scrollableLayout().setFlex(true);
  scrollableLayout().setPaddingLeft(20_px);
  scrollableLayout().setPaddingRight(20_px);
  scrollableLayout().setPaddingBottom(8_px);
  scrollableLayout().setFlexRows(false);
  scrollableLayout().setFlexWrap(true);

  for (auto &section : sections_)
    addScrolledChild(section.get());
}

ExamplesFrame::~ExamplesFrame() = default;

void ExamplesFrame::setupBars()
{
  bar_list_ = std::make_unique<observe::BarList>(kNumBars);

  bar_list_->onDraw() = [this](observe::Canvas &canvas)
  {
    canvas.setNativePixelScale();

    double render_time = canvas.time();
    float space = 1;
    float bar_width = (bar_list_->nativeWidth() + space) / kNumBars;
    int bar_height = bar_list_->nativeHeight();
    for (int i = 0; i < kNumBars; ++i)
    {
      float x = bar_width * i;
      float x_pos = std::round(x);
      float right = std::round(x + bar_width - space);
      float current_height = (sin1((render_time * 60.0 + i * 30) / 600.0f) + 1.0f) * 0.5f * bar_height;
      bar_list_->positionBar(i, x_pos, current_height, right - x_pos, bar_height - current_height);
    }
    bar_list_->draw(canvas);
  };
}

void ExamplesFrame::setupButtons()
{
  observe::Font font(20, resources::fonts::Lato_Regular_ttf);

  icon_button_ = std::make_unique<observe::ToggleIconButton>(resources::icons::check_circle_svg.data,
                                                            resources::icons::check_circle_svg.size, true);

  text_button_ = std::make_unique<observe::ToggleTextButton>("Toggle");
  text_button_->setFont(font);

  ui_button_ = std::make_unique<observe::UiButton>("Trigger Overlay");
  ui_button_->setFont(font);
  ui_button_->onToggle() = [this](observe::Button *button, bool toggled)
  {
    on_show_overlay_.callback();
  };

  ui_button_->setToggleOnMouseDown(true);

  action_button_ = std::make_unique<observe::UiButton>("Popup Menu");
  action_button_->setFont(font);
  action_button_->setActionButton();
  action_button_->onToggle() = [this](observe::Button *button, bool toggled)
  {
    observe::PopupMenu menu;
    menu.addOption(0, "Take Screenshot");
    menu.addOption(1, "Toggle Debug Info");
    menu.addBreak();
    observe::PopupMenu sub_menu("Sub Menu");
    sub_menu.addOption(3, "Sub Item 1");
    sub_menu.addBreak();
    sub_menu.addOption(4, "Sub Item 2");
    sub_menu.addOption(5, "Sub Item 3");
    sub_menu.addOption(6, "Sub Item 4");
    menu.addSubMenu(sub_menu);
    observe::PopupMenu sub_menu2("Other Sub Menu");
    sub_menu2.addOption(7, "Other Sub Item 1");
    sub_menu2.addBreak();
    sub_menu2.addOption(8, "Other Sub Item 2");
    sub_menu2.addOption(9, "Other Sub Item 3");
    sub_menu2.addOption(10, "Other Sub Item 4");
    menu.addSubMenu(sub_menu2);
    menu.addBreak();
    menu.addOption(12, "Force Crash");
    menu.addOption(13, "Close Application");

    menu.onSelection() = [this](int id)
    {
      if (id == 0)
      {
        observe::File file = observe::hostExecutable().parent_path() / "screenshot.png";
        on_screenshot_.callback(file.string());
      }
      else if (id == 1)
        on_toggle_debug_.callback();
      else if (id == 12)
        observe::forceCrash();
      else if (id == 13)
        observe::closeApplication();
    };

    menu.show(action_button_.get());
  };
  action_button_->setToggleOnMouseDown(true);

  icon_button_->layout().setHeight(40_px);
  action_button_->layout().setHeight(40_px);
  text_button_->layout().setHeight(40_px);
  ui_button_->layout().setHeight(40_px);

  button_container_.layout().setFlex(true);
  button_container_.layout().setFlexWrap(true);
  button_container_.layout().setFlexGap(16_px);
  button_container_.layout().setFlexItemAlignment(observe::Layout::ItemAlignment::Stretch);
  button_container_.layout().setFlexWrapAlignment(observe::Layout::WrapAlignment::Stretch);
  button_container_.layout().setPadding(16_px);
  button_container_.addChild(ui_button_.get());
  button_container_.addChild(action_button_.get());
  button_container_.addChild(icon_button_.get());
  button_container_.addChild(text_button_.get());
}

void ExamplesFrame::setupShapes()
{
  shapes_ = std::make_unique<observe::Frame>();
  shapes_->onDraw() = [this](observe::Canvas &canvas)
  {
    double render_time = canvas.time();

    float center_radians = render_time * 1.2f;
    double phase = render_time * 0.1;
    float radians = kHalfPi * sin1(phase) + kHalfPi;

    int min_shape_padding = 20.0f;
    int shape_width = std::min(shapes_->width() / 5, shapes_->height() / 2) - min_shape_padding;
    int shape_padding_x = shapes_->width() / 5 - shape_width;
    int shape_padding_y = shapes_->height() / 2 - shape_width;

    int shape_x = shape_padding_x / 2;
    int shape_y = 0;
    int shape_y2 = shape_y + shape_width + shape_padding_y;
    int roundness = shape_width / 8;

    float shape_phase = canvas.time() * 0.1f;
    shape_phase -= std::floor(shape_phase);
    float shape_cycle = sin1(shape_phase) * 0.5f + 0.5f;
    float thickness = shape_width * shape_cycle / 8.0f + 1.0f;

    canvas.setColor(ShapeColor);
    canvas.rectangle(shape_x, shape_y, shape_width, shape_width);
    canvas.rectangleBorder(shape_x, shape_y2, shape_width, shape_width, thickness);
    canvas.circle(shape_x + shape_width + shape_padding_x, shape_y, shape_width);
    canvas.ring(shape_x + shape_width + shape_padding_x, shape_y2, shape_width, thickness);
    canvas.roundedRectangle(shape_x + 2 * (shape_width + shape_padding_x), shape_y, shape_width,
                            shape_width, roundness);
    canvas.roundedRectangleBorder(shape_x + 2 * (shape_width + shape_padding_x), shape_y2,
                                  shape_width, shape_width, roundness, thickness);
    canvas.arc(shape_x + 3.0f * (shape_width + shape_padding_x), shape_y, shape_width, thickness,
               center_radians, radians, false);
    canvas.arc(shape_x + 3.0f * (shape_width + shape_padding_x), shape_y2, shape_width, thickness,
               center_radians, radians, true);

    float max_separation = shape_width / 4.0f;
    float separation = shape_cycle * max_separation;
    int triangle_x = shape_x + 4 * (shape_width + shape_padding_x) + max_separation;
    int triangle_y = shape_y + max_separation;
    float triangle_width = (shape_width - 2.0f * max_separation) / 2.0f;
    canvas.triangleDown(triangle_x, triangle_y - separation, triangle_width);
    canvas.triangleRight(triangle_x - separation, triangle_y, triangle_width);
    canvas.triangleUp(triangle_x, triangle_y + triangle_width + separation, triangle_width);
    canvas.triangleLeft(triangle_x + triangle_width + separation, triangle_y, triangle_width);

    float segment_x = shape_x + 4 * (shape_width + shape_padding_x);
    float segment_y = shape_y2;
    float shape_radius = shape_width / 2;
    std::pair<float, float> segment_positions[] = {{segment_x, segment_y + shape_radius},
                                                   {segment_x + shape_radius, segment_y},
                                                   {segment_x + shape_width, segment_y + shape_radius},
                                                   {segment_x + shape_radius, segment_y + shape_width}};

    int index = std::min<int>(shape_phase * 4.0f, 3);
    float movement_phase = shape_phase * 4.0f - index;
    float t1 = sin1(std::min(movement_phase * 0.5f, 0.25f) - 0.25f) + 1.0f;
    float t2 = sin1(std::max(movement_phase * 0.5f, 0.25f) - 0.25f);
    std::pair<float, float> from = segment_positions[index];
    std::pair<float, float> to = segment_positions[(index + 1) % 4];
    float delta_x = to.first - from.first;
    float delta_y = to.second - from.second;
    canvas.segment(from.first + delta_x * t1, from.second + delta_y * t1, from.first + delta_x * t2,
                   from.second + delta_y * t2, thickness, true);

    from = segment_positions[(index + 2) % 4];
    to = segment_positions[(index + 3) % 4];
    delta_x = to.first - from.first;
    delta_y = to.second - from.second;
    canvas.segment(from.first + delta_x * t1, from.second + delta_y * t1, from.first + delta_x * t2,
                   from.second + delta_y * t2, thickness, false);

    shapes_->redraw();
  };
}

void ExamplesFrame::setupTextEditors()
{
  observe::Font font(20, resources::fonts::Lato_Regular_ttf);

  left_text_editor_ = std::make_unique<observe::TextEditor>();
  left_text_editor_->setJustification(observe::Font::kLeft);
  left_text_editor_->setDefaultText("Left Text");
  left_text_editor_->layout().setHeight(40_px);
  left_text_editor_->layout().setFlexGrow(1.0f);
  left_text_editor_->setFont(font);

  center_editor_ = std::make_unique<observe::TextEditor>();
  center_editor_->setDefaultText("Center Text");
  center_editor_->layout().setHeight(40_px);
  center_editor_->layout().setFlexGrow(1.0f);
  center_editor_->setFont(font);

  right_text_editor_ = std::make_unique<observe::TextEditor>();
  right_text_editor_->setJustification(observe::Font::kRight);
  right_text_editor_->setDefaultText("Right Text");
  right_text_editor_->layout().setHeight(40_px);
  right_text_editor_->layout().setFlexGrow(1.0f);
  right_text_editor_->setFont(font);

  text_editor_ = std::make_unique<observe::TextEditor>();
  text_editor_->setDefaultText("Multiline Text");
  text_editor_->layout().setHeight(100_px);
  text_editor_->layout().setFlexGrow(1.0f);
  text_editor_->setMultiLine(true);
  text_editor_->setJustification(observe::Font::kTopLeft);
  text_editor_->setFont(font);

  text_editor_container_.layout().setFlex(true);
  text_editor_container_.layout().setFlexWrap(true);
  text_editor_container_.layout().setFlexGap(16_px);
  text_editor_container_.layout().setFlexWrapAlignment(observe::Layout::WrapAlignment::Stretch);
  text_editor_container_.addChild(left_text_editor_.get());
  text_editor_->layout().setHeight(100_px);
  text_editor_container_.addChild(center_editor_.get());
  text_editor_container_.addChild(right_text_editor_.get());
  text_editor_container_.addChild(text_editor_.get());
}