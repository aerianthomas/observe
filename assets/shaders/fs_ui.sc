$input v_texcoord0, v_texcoord1, v_color0

#include "bgfx_shader.sh"

// Signed distance from a point to an axis-aligned rounded box centered at
// the origin (Inigo Quilez's formula). All units are pixels, in the
// shape's own local space - see Canvas::addRoundedRectVertices() and
// Canvas::drawLine() for how a_texcoord0/a_texcoord1 get built on the CPU
// side, including for shapes (like lines) that are rotated on screen.
//
//   centerPos: fragment position relative to the shape's center
//   halfSize:  half-width/half-height of the (unrounded) box
//   radius:    corner radius - 0 gives a plain sharp-cornered rect,
//              radius == min(halfSize.x, halfSize.y) gives a circle/pill
float roundedBoxSDF(vec2 centerPos, vec2 halfSize, float radius)
{
    vec2 q = abs(centerPos) - halfSize + radius;

    return
        min(max(q.x, q.y), 0.0)
        + length(max(q, vec2_splat(0.0)))
        - radius;
}

void main()
{
    float dist =
        roundedBoxSDF(
            v_texcoord0,
            v_texcoord1.xy,
            v_texcoord1.z);

    // Antialiased edge: fully opaque a pixel or more inside the shape,
    // fully transparent a pixel or more outside it, smooth in between.
    float alpha = 1.0 - smoothstep(-1.0, 1.0, dist);

    gl_FragColor = vec4(v_color0.rgb, v_color0.a * alpha);
}
