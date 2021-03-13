#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include "../include/Shader.h"
#include "../include/Logging.h"
#include "../include/Utils.h"
#include "../include/ShaderCompiler/Compiler.h"

Starsurge::Uniform::Uniform(std::string t_name, std::string t_type) : name(t_name), type(t_type) {
    // Set the default value.
    if (type == "bool") { SetData(false); }
    else if (type == "int") { SetData((int)0); }
    else if (type == "uint") { SetData((unsigned int)0); }
    else if (type == "float") { SetData(0.0f); }
    else if (type == "vec2" || type == "ivec2") { SetData(Vector2(0,0)); }
    else if (type == "vec3" || type == "ivec3") { SetData(Vector3(0,0,0)); }
    else if (type == "vec4" || type == "ivec4") { SetData(Vector4(0,0,0,0)); }
    else if (type == "color") { SetData(Colors::WHITE); }
    else if (type == "sampler2D") { SetData((Texture*)NULL); }
}

std::string Starsurge::Uniform::GetName() {
    return this->name;
}

std::string Starsurge::Uniform::GetType() {
    return this->type;
}

void Starsurge::Uniform::SetData(bool val) {
    if (GetType() != "bool") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type bool.");
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(int val) {
    if (GetType() != "int") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type int.");
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(unsigned int val) {
    if (GetType() != "uint") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type uint.");
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(float val) {
    if (GetType() != "float") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type float.");
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Vector2 val) {
    if (GetType() != "vec2") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type vec2.");
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Vector3 val) {
    if (GetType() != "vec3") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type vec3.");
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Vector4 val) {
    if (GetType() != "vec4") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type vec4.");
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Color val) {
    if (GetType() != "color") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type color.");
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Texture * val) {
    if (GetType() != "sampler2D") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type Texture*.");
    }
    this->data = val;
}

Starsurge::Shader::Shader(std::string source_code) {
    SetCode(source_code);
}

void Starsurge::Shader::SetCode(std::string source_code) {
    this->code = source_code;
    this->passes.clear();
    this->needs_recompiling = true;
}

Starsurge::Shader::~Shader() {
    for (unsigned int i = 0; i < passes.size(); ++i) {
        glDeleteShader(passes[i].vertex);
        glDeleteShader(passes[i].fragment);
    }
    passes.clear();
}

unsigned int Starsurge::Shader::NumberOfPasses() {
    return passes.size();
}

unsigned int Starsurge::Shader::GetProgram(unsigned int i) {
    return passes[i].program;
}

std::vector<Starsurge::Uniform> Starsurge::Shader::GetUniforms() {
    return this->uniforms;
}

bool Starsurge::Shader::Compile() {
    if (!this->needs_recompiling) {
        return true;
    }

    // Get rid of old shader data.
    for (unsigned int i = 0; i < passes.size(); ++i) {
        glDeleteShader(passes[i].vertex);
        glDeleteShader(passes[i].fragment);
    }
    passes.clear();
    uniforms.clear();

    GLSL::Compiler compiler(code);
    bool success = compiler.Compile(this);
    this->needs_recompiling = false;

    return success;
}

void Starsurge::Shader::Use(unsigned int pass) {
    if (this->needs_recompiling) {
        Compile();
    }
    glUseProgram(passes[pass].program);
}
