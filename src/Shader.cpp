#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include "../include/Shader.h"
#include "../include/Logging.h"

Starsurge::Shader::Shader(std::string source_code) : code(source_code) {
    this->needs_recompiling = true;
}

Starsurge::Shader::~Shader() {
    glDeleteShader(this->vertexShader);
    glDeleteShader(this->fragmentShader);
}

std::map<std::string, std::string> parse_code(std::string entire_code) {
    int bracketCount = 0;
    std::string tag = "";
    std::string code = "";
    std::map<std::string, std::string> table;

    for (unsigned int i = 0; i < entire_code.length(); ++i) {
        char c = entire_code[i];

        if (bracketCount == 0 && c != '{' && c != ' ' && c != '\t' && c != '\n') {
            tag += c;
        }
        else if (c == '{') {
            if (bracketCount > 0) { code += c; }
            bracketCount++;
        }
        else if (c == '}') {
            if (bracketCount > 1) { code += c; }
            bracketCount--;
            if (bracketCount == 0) {
                table[tag] = code;
                tag = "";
                code = "";
            }
            else {

            }
        }
        else {
            code += c;
        }
    }

    return table;
}

void Starsurge::Shader::Compile() {
    if (!this->needs_recompiling) {
        return;
    }

    int success; // Used when getting compilation and link status.
    char infoLog[512];
    std::map<std::string, std::string> table = parse_code(this->code);

    std::string vert_code = "#version 330 core\n"
        "layout (location = 0) in vec3 VERTEX;\n\n\0";
    vert_code += table["vertex"];
    vert_code += "void main() {\n"
        "   gl_Position = vertex();\n"
        "}\0";
    const char * vert_code_c_str = vert_code.c_str();
    Log(vert_code_c_str);

    std::string frag_code = "#version 330 core\n"
        "out vec4 FragColor;\n\n\0";
    frag_code += table["fragment"];
    frag_code += "void main() {\n"
        "   FragColor = fragment();\n"
        "}\0";
    const char * frag_code_c_str = frag_code.c_str();
    Log("======");
    Log(frag_code_c_str);

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
