#include "shader_quad.h"

#include "modules/observe_graphics/theme.h"

namespace observe
{
  OBSERVE_THEME_COLOR(ShaderQuadColor, 0xffffffff);

  ShaderQuad::ShaderQuad(const EmbeddedFile &vertex_shader, const EmbeddedFile &fragment_shader,
                         BlendMode state) : shader_(vertex_shader, fragment_shader, state) {}

  void ShaderQuad::draw(Canvas &canvas)
  {
    canvas.setColor(ShaderQuadColor);
    canvas.shader(&shader_, 0.0f, 0.0f, width(), height());
    redraw();
  }
}