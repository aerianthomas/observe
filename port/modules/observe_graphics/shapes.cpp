#include "shapes.h"

#include "embedded/shaders.h"
#include "layer.h"
#include "region.h"

#define OBSERVE_SET_PROGRAM(shape, vertex, fragment) \
  const EmbeddedFile &shape::vertexShader()         \
  {                                                 \
    return vertex;                                  \
  }                                                 \
  const EmbeddedFile &shape::fragmentShader()       \
  {                                                 \
    return fragment;                                \
  }

namespace observe
{
  OBSERVE_SET_PROGRAM(Fill, shaders::vs_color, shaders::fs_color)
  OBSERVE_SET_PROGRAM(Rectangle, shaders::vs_shape, shaders::fs_rectangle)
  OBSERVE_SET_PROGRAM(RoundedRectangle, shaders::vs_shape, shaders::fs_rounded_rectangle)
  OBSERVE_SET_PROGRAM(Circle, shaders::vs_shape, shaders::fs_circle)
  OBSERVE_SET_PROGRAM(Squircle, shaders::vs_shape, shaders::fs_squircle)
  OBSERVE_SET_PROGRAM(FlatArc, shaders::vs_arc, shaders::fs_flat_arc)
  OBSERVE_SET_PROGRAM(RoundedArc, shaders::vs_arc, shaders::fs_rounded_arc)
  OBSERVE_SET_PROGRAM(FlatSegment, shaders::vs_complex_shape, shaders::fs_flat_segment)
  OBSERVE_SET_PROGRAM(RoundedSegment, shaders::vs_complex_shape, shaders::fs_rounded_segment)
  OBSERVE_SET_PROGRAM(Triangle, shaders::vs_complex_shape, shaders::fs_triangle)
  OBSERVE_SET_PROGRAM(QuadraticBezier, shaders::vs_complex_shape, shaders::fs_quadratic_bezier)
  OBSERVE_SET_PROGRAM(Diamond, shaders::vs_shape, shaders::fs_diamond)
  OBSERVE_SET_PROGRAM(ImageWrapper, shaders::vs_tinted_texture, shaders::fs_tinted_texture)
  OBSERVE_SET_PROGRAM(PathFillWrapper, shaders::vs_sample_path, shaders::fs_sample_path)
  OBSERVE_SET_PROGRAM(GraphLineWrapper, shaders::vs_shape, shaders::fs_graph_line)
  OBSERVE_SET_PROGRAM(GraphFillWrapper, shaders::vs_shape, shaders::fs_graph_fill)
  OBSERVE_SET_PROGRAM(HeatMapWrapper, shaders::vs_shape, shaders::fs_heat_map)
  OBSERVE_SET_PROGRAM(SampleRegion, shaders::vs_post_effect, shaders::fs_post_effect)

  SampleRegion::SampleRegion(const ClampBounds &clamp, const PackedBrush *brush, float x, float y,
                             float width, float height, const Region *region, PostEffect *post_effect) : Shape(region->layer(), clamp, brush, x, y, width, height), region(region),
                                                                                                         post_effect(post_effect)
  {
    if (post_effect)
      batch_id = post_effect;
  }

  void SampleRegion::setVertexData(Vertex *vertices) const
  {
    region->layer()->setTexturePositionsForRegion(region, vertices);
  }
}
