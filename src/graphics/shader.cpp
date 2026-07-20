#include <observe/graphics/shader.h>
#include <observe/graphics/shader_loader.h>

#include <bgfx/bgfx.h>

#include <iostream>

namespace observe
{

    Shader::~Shader()
    {
        destroy();
    }

    Shader::Shader(Shader &&other) noexcept
    {
        m_program = other.m_program;
        other.m_program = BGFX_INVALID_HANDLE;
    }

    Shader &Shader::operator=(Shader &&other) noexcept
    {
        if (this != &other)
        {
            destroy();

            m_program = other.m_program;
            other.m_program = BGFX_INVALID_HANDLE;
        }

        return *this;
    }

    bool Shader::load(
        const std::filesystem::path &vertexShader,
        const std::filesystem::path &fragmentShader)
    {
        auto vs = loadShader(vertexShader);
        auto fs = loadShader(fragmentShader);

        if (!bgfx::isValid(vs) ||
            !bgfx::isValid(fs))

            return false;

        m_program =
            bgfx::createProgram(
                vs,
                fs,
                true);

        if (!bgfx::isValid(m_program))
        {
            std::cout << "Failed creating bgfx program\n";
            return false;
        }

        return true;
    }

    void Shader::destroy()
    {
        if (bgfx::isValid(m_program))
        {
            bgfx::destroy(m_program);
            m_program = BGFX_INVALID_HANDLE;
        }
    }

}