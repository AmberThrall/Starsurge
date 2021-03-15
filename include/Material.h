#pragma once
#include <variant>
#include <map>
#include "Shader.h"
#include "Vector.h"
#include "Color.h"

namespace Starsurge {
    class Material {
    public:
        Material();
        Material(Shader * t_shader);

        void SetShader(Shader * t_shader);
        Shader * GetShader();

        Uniform * GetUniform(int pass, std::string name);
        void Apply(unsigned int passno);
    protected:
        Shader * shader;
    private:
        void SetupData();
        std::map<std::pair<int,std::string>, Uniform> data;
    };
}
