#pragma once

#include <bgfx/bgfx.h>

#include <cstdint>
#include <vector>

#include <observe/vertex_layout.h>

namespace observe
{

    class Mesh
    {
    public:
        Mesh() = default;

        ~Mesh();

        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;

        bool create(
            const void *vertices,
            uint32_t vertexCount,
            const VertexLayout &layout);

        void destroy();

        void draw() const;

    private:
        bgfx::VertexBufferHandle m_vertexBuffer =
            BGFX_INVALID_HANDLE;

        uint32_t m_vertexCount = 0;
    };

}