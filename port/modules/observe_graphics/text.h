#pragma once

#include "font.h"
#include "modules/observe_utils/string_utils.h"

namespace observe
{
  class Canvas;

  class Text
  {
  public:
    Text() = default;
    explicit Text(const String &text, const Font &font,
                  Font::Justification justification = Font::kCenter, bool multi_line = false) : text_(text), font_(font), justification_(justification), multi_line_(multi_line) {}
    virtual ~Text() = default;

    void setText(const String &text) { text_ = text; }
    const String &text() const { return text_; }

    void setFont(const Font &font) { font_ = font; }
    const Font &font() const { return font_; }

    void setJustification(Font::Justification justification) { justification_ = justification; }
    Font::Justification justification() const { return justification_; }

    void setMultiLine(bool multi_line) { multi_line_ = multi_line; }
    bool multiLine() const { return multi_line_; }

    void setCharacterOverride(int character) { character_override_ = character; }
    int characterOverride() const { return character_override_; }

  private:
    String text_;
    Font font_;
    Font::Justification justification_ = Font::kCenter;
    bool multi_line_ = false;
    int character_override_ = 0;
  };
}