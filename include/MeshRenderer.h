#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"

namespace Starsurge {
    class MeshRenderer : public Component {
    public:
        MeshRenderer(Mesh * t_mesh, Material * t_mat);

        void Render();
    private:
        Mesh * mesh;
        Material * material;
    };
}
