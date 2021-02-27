#pragma once
#include <string>

namespace Starsurge {
    class Shader {
    public:
        Shader(std::string source_code);
        ~Shader();

        void Compile();
        void Use();
    private:
        std::string code;
        unsigned int shaderProgram;
        unsigned int vertexShader;
        unsigned int fragmentShader;
        bool needs_recompiling;
    };

    namespace Shaders {
        extern Shader BasicShader;
    }
}
