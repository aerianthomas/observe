#pragma once

#include "modules/observe_graphics/shader.h"
#include "modules/observe_ui/frame.h"

namespace observe
{
  class ShaderQuad : public Frame
  {
  public:
    ShaderQuad(const EmbeddedFile &vertex_shader, const EmbeddedFile &fragment_shader, BlendMode state);
    ~ShaderQuad() override = default;

    void draw(Canvas &canvas) override;

  private:
    Shader shader_;

    OBSERVE_LEAK_CHECKER(ShaderQuad)
  };
}