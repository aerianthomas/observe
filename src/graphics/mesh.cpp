#include <observe/graphics/mesh.h>

namespace observe
{

    Mesh::~Mesh()
    {
        destroy();
    }

    bool Mesh::create(
        const void *vertices,
        uint32_t vertexCount,
        const VertexLayout &layout)
    {
        const bgfx::Memory *memory =
            bgfx::copy(
                vertices,
                vertexCount *
                    layout.get().getStride());

        m_vertexBuffer =
            bgfx::createVertexBuffer(
                memory,
                layout.get());

        if (!bgfx::isValid(m_vertexBuffer))
            return false;

        m_vertexCount = vertexCount;

        return true;
    }

    void Mesh::destroy()
    {
        if (bgfx::isValid(m_vertexBuffer))
        {
            bgfx::destroy(m_vertexBuffer);

            m_vertexBuffer =
                BGFX_INVALID_HANDLE;
        }
    }

    void Mesh::draw() const
    {
        if (!bgfx::isValid(m_vertexBuffer))
            return;

        bgfx::setVertexBuffer(
            0,
            m_vertexBuffer);
    }

}