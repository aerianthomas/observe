#include <observe/shader_loader.h>

#include <fstream>
#include <vector>

namespace observe
{

    bgfx::ShaderHandle loadShader(
        const std::filesystem::path &path)
    {
        std::ifstream file(
            path,
            std::ios::binary | std::ios::ate);

        if (!file)
            return BGFX_INVALID_HANDLE;

        const auto size = file.tellg();

        file.seekg(0, std::ios::beg);

        std::vector<char> data(
            static_cast<size_t>(size));

        file.read(
            data.data(),
            size);

        const bgfx::Memory *mem =
            bgfx::copy(
                data.data(),
                static_cast<uint32_t>(data.size()));

        return bgfx::createShader(mem);
    }

}