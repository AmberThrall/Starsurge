#pragma once
#include <string>
#include <vector>
#include <variant>
#include "Shader.h"
#include "Vector.h"
#include "Color.h"
#include "Texture.h"

namespace Starsurge {
    namespace GLSL {
        class Compiler;
    }
    struct ShaderPass {
        unsigned int vertex;
        unsigned int fragment;
        unsigned int program;
    };

    class Uniform {
    public:
        Uniform() {};
        Uniform(std::string t_name, std::string t_type);
        std::string GetName();
        std::string GetType();

        template<typename T>
        const T GetData() {
            //TODO: Check against type.
            return std::get<T>(data);
        }

        void SetData(bool val);
        void SetData(int val);
        void SetData(unsigned int val);
        void SetData(float val);
        void SetData(double val);
        void SetData(Vector2 val);
        void SetData(Vector3 val);
        void SetData(Vector4 val);
        void SetData(Color val);
        void SetData(Texture * val);
    private:
        std::string name;
        std::string type;
        std::variant<bool, int, unsigned int, float, Vector2, Vector3, Vector4, Color, Texture*> data;
    };

    class Shader {
    public:
        Shader(std::string source_code);
        ~Shader();
        void SetCode(std::string source_code);

        unsigned int NumberOfPasses();
        unsigned int GetProgram(unsigned int i);

        std::vector<Uniform> GetUniforms();
        bool Compile();
        void Use(unsigned int pass);
    private:
        std::string code;
        bool needs_recompiling;
        std::vector<ShaderPass> passes;
        std::vector<Uniform> uniforms;

        friend class GLSL::Compiler;
    };

    namespace Shaders {
        extern Shader BasicShader;
    }
}
