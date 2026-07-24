#pragma once

namespace observe
{
  struct Uniforms
  {
    static constexpr char kTime[] = "u_time";
    static constexpr char kMult[] = "u_mult";
    static constexpr char kTextureClamp[] = "u_texture_clamp";
    static constexpr char kBounds[] = "u_bounds";
    static constexpr char kColor[] = "u_color";
    static constexpr char kColorMult[] = "u_color_mult";
    static constexpr char kLimitMult[] = "u_limit_mult";
    static constexpr char kOriginFlip[] = "u_origin_flip";
    static constexpr char kAtlasScale[] = "u_atlas_scale";
    static constexpr char kAtlasScale2[] = "u_atlas_scale2";
    static constexpr char kCenterPosition[] = "u_center_position";
    static constexpr char kDimensions[] = "u_dimensions";
    static constexpr char kLineWidth[] = "u_line_width";
    static constexpr char kResampleValues[] = "u_resample_values";
    static constexpr char kResampleValues2[] = "u_resample_values2";
    static constexpr char kThreshold[] = "u_threshold";
    static constexpr char kPixelSize[] = "u_pixel_size";
    static constexpr char kGradient[] = "s_gradient";
    static constexpr char kTexture[] = "s_texture";
    static constexpr char kTexture2[] = "s_texture2";
    static constexpr char kGradientTexturePosition[] = "u_gradient_texture_position";
    static constexpr char kGradientPosition[] = "u_gradient_position";
    static constexpr char kGradientPosition2[] = "u_gradient_position2";
    static constexpr char kRadialGradient[] = "u_radial_gradient";
  };
}
