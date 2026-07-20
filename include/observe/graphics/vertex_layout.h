#pragma once

#include <bgfx/bgfx.h>

namespace observe
{

    class VertexLayout
    {
    public:
        VertexLayout();

        const bgfx::VertexLayout &get() const noexcept
        {
            return m_layout;
        }

    private:
        bgfx::VertexLayout m_layout;
    };

}