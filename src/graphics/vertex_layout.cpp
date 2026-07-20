#include <observe/graphics/vertex_layout.h>

namespace observe
{

    VertexLayout::VertexLayout()
    {
        m_layout
            .begin()
            .add(
                bgfx::Attrib::Position,
                3,
                bgfx::AttribType::Float)
            .end();
    }

}