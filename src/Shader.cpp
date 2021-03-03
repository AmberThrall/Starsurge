#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include "../include/Shader.h"
#include "../include/Logging.h"
#include "../include/Utils.h"

Starsurge::Shader::Shader(std::string source_code) : code(source_code) {
    this->needs_recompiling = true;
    ParseUniforms();
}

Starsurge::Shader::~Shader() {
    glDeleteShader(this->vertexShader);
    glDeleteShader(this->fragmentShader);
}

const std::map<std::string, std::string> Starsurge::Shader::GetUniforms() {
    return this->uniforms;
}

void Starsurge::Shader::ParseUniforms() {
    this->uniforms.clear();
    //TODO: Arrays, Blocks

    std::vector<std::string> commands = Explode(this->code, ';');
    for (unsigned int i = 0; i < commands.size(); ++i) {
        std::string ltrimmedCommand = LTrim(commands[i]);
        if (ltrimmedCommand.substr(0,7) == "uniform") {
            std::vector<std::string> command_exploded = ExplodeWhitespace(ltrimmedCommand, false);
            // There should be three parts uniform <type> <name>
            if (command_exploded.size() != 3) {
                ShaderError("Unknown uniform command "+ltrimmedCommand);
                continue;
            }
            // Check that it is a valid type
            if (!ElemOf<const char*>(VALID_UNIFORM_TYPES, VALID_UNIFORM_TYPES_COUNT, command_exploded[1].c_str())) {
                ShaderError("Unknown or incompatible uniform type '"+command_exploded[1]+"'.");
                continue;
            }
            this->uniforms[command_exploded[2]] = command_exploded[1]; //TODO: Sanity check name.
        }
    }
}

void Starsurge::Shader::Compile() {
    if (!this->needs_recompiling) {
        return;
    }

    int success; // Used when getting compilation and link status.
    char infoLog[512];

    std::string vert_code = "#version 330 core\n"
        "layout (location = 0) in vec3 _internal_Position;\n"
        "\n"
        "struct VertexData {\n"
        "   vec3 Position;\n"
        "};\n\n\0";
    vert_code += this->code;
    vert_code += "\n"
        "void main() {\n"
        "   VertexData vertexData;\n"
        "   vertexData.Position = _internal_Position;\n"
        "   gl_Position = vertex(vertexData);\n"
        "}\0";
    const char * vert_code_c_str = vert_code.c_str();

    std::string frag_code = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "struct VertexData {\n"
        "   vec3 Position;\n"
        "};\n\n\0";
    frag_code += this->code;
    frag_code += "void main() {\n"
        "   FragColor = fragment();\n"
        "}\0";
    const char * frag_code_c_str = frag_code.c_str();

    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vertexShader, 1, &vert_code_c_str, NULL);
    glCompileShader(this->vertexShader);
    glGetShaderiv(this->vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(this->vertexShader, 512, NULL, infoLog);
        ShaderError(infoLog);
    }

    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fragmentShader, 1, &frag_code_c_str, NULL);
    glCompileShader(this->fragmentShader);
    glGetShaderiv(this->fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(this->fragmentShader, 512, NULL, infoLog);
        ShaderError(infoLog);
    }

    this->shaderProgram = glCreateProgram();
    glAttachShader(this->shaderProgram, this->vertexShader);
    glAttachShader(this->shaderProgram, this->fragmentShader);
    glLinkProgram(this->shaderProgram);
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->shaderProgram, 512, NULL, infoLog);
        ShaderError(infoLog);
    }

    this->needs_recompiling = false;
}

void Starsurge::Shader::Use() {
    if (this->needs_recompiling) {
        Compile();
    }
    glUseProgram(this->shaderProgram);
}

unsigned int Starsurge::Shader::GetProgram() {
    return this->shaderProgram;
}
