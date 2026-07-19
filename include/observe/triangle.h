#pragma once

#include <observe/mesh.h>
#include <observe/shader.h>
#include <observe/vertex_layout.h>
#include <observe/math/transform.h>

namespace observe
{

    class Triangle
    {
    public:
        bool initialize();

        void draw();

    private:
        VertexLayout m_layout;
        Mesh m_mesh;
        Shader m_shader;
        Transform m_transform;
    };

}