#pragma once

#include <bgfx/bgfx.h>

#include <filesystem>

namespace observe
{

    bgfx::ShaderHandle loadShader(
        const std::filesystem::path &path);

}