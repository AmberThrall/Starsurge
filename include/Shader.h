#pragma once
#include <string>
#include <map>
#include <vector>

namespace Starsurge {
    static const char* VALID_UNIFORM_TYPES[] = { "bool", "int", "uint", "float", "double", "bvec2", "bvec3", "bvec4",
        "ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4", "vec2", "vec3", "vec4", "dvec2", "dvec3", "dvec4",
        "mat2x2", "mat2x3", "mat2x4", "mat3x2", "mat3x3", "mat3x4", "mat4x2", "mat4x3", "mat4x4", "mat2", "mat3", "mat4"
    };
    static unsigned int VALID_UNIFORM_TYPES_COUNT = 32;

    class Shader {
    public:
        Shader(std::string source_code);
        ~Shader();

        const std::map<std::string, std::string> GetUniforms();
        void Compile();
        void Use();

        unsigned int GetProgram();
    private:
        void ParseUniforms();

        std::string code;
        unsigned int shaderProgram;
        unsigned int vertexShader;
        unsigned int fragmentShader;
        bool needs_recompiling;
        std::map<std::string, std::string> uniforms;
    };

    namespace Shaders {
        extern Shader BasicShader;
    }
}
