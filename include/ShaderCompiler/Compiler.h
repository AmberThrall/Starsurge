#pragma once

#include "Parser.h"

namespace Starsurge {
    class Shader;
    struct ShaderPass;
    class Uniform;

    namespace GLSL {
        struct CodeStructure {
            std::string shaderType;
            std::vector<std::string> passes;
            std::vector<unsigned int> codeOffset;
        };

        class Compiler {
        public:
            Compiler(std::string source_code = "");

            void SetCode(std::string source_code);
            bool Compile(Shader * shader);
        private:
            void HandleGLSLErrors(int linenoOffset, std::string log);
            std::string TranslateCode(ASTNode * root, std::string entry);
            bool CompileVertex(ShaderPass & shaderPass, unsigned int linenoOffset, std::string vert_code);
            bool CompileFragment(ShaderPass & shaderPass, unsigned int linenoOffset, std::string frag_code, bool postprocessingeffect);
            void Error(unsigned int lineno, std::string msg);
            CodeStructure ParseStructure();
            void ParseUniform(ShaderPass & shaderPass, ASTNodeVariableDeclaration * var);
            void BuildDeps(ASTNode* node, std::vector<std::string> & deps);
            std::string Node2Code(ASTNode * node);

            std::string code;
            std::vector<std::string> errors;
        };
    }
}
