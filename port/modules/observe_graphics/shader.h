#pragma once

#include "graphics_utils.h"
#include "modules/observe_file_embed/embedded_file.h"

namespace observe
{
  class Canvas;

  class Shader
  {
  public:
    Shader() = delete;
    Shader(const EmbeddedFile &vertex_shader, const EmbeddedFile &fragment_shader, BlendMode state) : vertex_shader_(vertex_shader), fragment_shader_(fragment_shader), state_(state) {}
    virtual ~Shader() = default;

    const EmbeddedFile &vertexShader() const { return vertex_shader_; }
    const EmbeddedFile &fragmentShader() const { return fragment_shader_; }
    BlendMode state() const { return state_; }

  private:
    EmbeddedFile vertex_shader_;
    EmbeddedFile fragment_shader_;
    BlendMode state_ = BlendMode::Alpha;
  };
}
