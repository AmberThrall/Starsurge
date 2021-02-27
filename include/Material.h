#pragma once
#include "Shader.h"

namespace Starsurge {
    class Material {
    public:
        Material();
        Material(Shader * t_shader);

        Shader * GetShader();
    protected:
        Shader * shader;
    };
}
