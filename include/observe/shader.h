#pragma once

#include <bgfx/bgfx.h>

#include <filesystem>
#include <string>

namespace observe
{

    class Shader
    {
    public:
        Shader() = default;
        ~Shader();

        Shader(const Shader &) = delete;
        Shader &operator=(const Shader &) = delete;

        Shader(Shader &&other) noexcept;
        Shader &operator=(Shader &&other) noexcept;

        bool load(const std::filesystem::path &vertexShader,
                  const std::filesystem::path &fragmentShader);

        void destroy();

        [[nodiscard]]
        bgfx::ProgramHandle handle() const noexcept
        {
            return m_program;
        }

        [[nodiscard]]
        bool valid() const noexcept
        {
            return bgfx::isValid(m_program);
        }

    private:
        bgfx::ProgramHandle m_program = BGFX_INVALID_HANDLE;
    };

}