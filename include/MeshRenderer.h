#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"

namespace Starsurge {
    class MeshRenderer : public Component {
    public:
        MeshRenderer(Mesh * t_mesh, Material * t_mat);

        void SetWireframe(bool wire);

        void Render();
    private:
        bool wireframe;
        Mesh * mesh;
        Material * material;
    };
}
