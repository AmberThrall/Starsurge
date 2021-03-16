#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include "../include/AssetManager.h"
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
    else if (type == "sampler2D") { SetData(AssetManager::Inst().GetTexture("Builtin/White")); }
    else if (type == "mat2" || type == "mat2x2") { SetData(Matrix2::Identity()); }
    else if (type == "mat3" || type == "mat3x3") { SetData(Matrix3::Identity()); }
    else if (type == "mat4" || type == "mat4x4") { SetData(Matrix4::Identity()); }
    else { Error("Unsupported uniform type: "+type); }
}

std::string Starsurge::Uniform::GetName() {
    return this->name;
}

std::string Starsurge::Uniform::GetType() {
    return this->type;
}

void Starsurge::Uniform::SetData(bool val) {
    if (GetType() != "bool") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type bool.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(int val) {
    if (GetType() != "int") {
        if (GetType() == "uint") { SetData((unsigned int)std::abs(val)); return; }
        if (GetType() == "float") { SetData((float)val); return; }
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type int.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(unsigned int val) {
    if (GetType() != "uint") {
        if (GetType() == "int") { SetData((int)val); return; }
        if (GetType() == "float") { SetData((float)val); return; }
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type uint.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(float val) {
    if (GetType() != "float") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type float.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Vector2 val) {
    if (GetType() != "vec2" && GetType() != "ivec2") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type Vector2.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Vector3 val) {
    if (GetType() != "vec3" && GetType() != "ivec3") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type Vector3.");
        return;
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Vector4 val) {
    if (GetType() != "vec4" && GetType() != "ivec4") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type Vector4.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Color val) {
    if (GetType() != "color") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type Color.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Texture * val) {
    if (GetType() != "sampler2D") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type Texture*.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Matrix2 val) {
    if (GetType() != "mat2" && GetType() != "mat2x2") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type Matrix2.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Matrix3 val) {
    if (GetType() != "mat3" && GetType() != "mat3x3") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type Matrix3.");
        return;
    }
    this->data = val;
}

void Starsurge::Uniform::SetData(Matrix4 val) {
    if (GetType() != "mat4" && GetType() != "mat4x4") {
        ShaderError("Tried to set uniform '"+this->name+"' of type "+GetType()+" to type Matrix4.");
        return;
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

std::map<std::string, Starsurge::Uniform> Starsurge::Shader::GetUniforms(unsigned int pass) {
    return passes[pass].uniforms;
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

    GLSL::Compiler compiler(code);
    bool success = compiler.Compile(this);
    this->needs_recompiling = false;

    // If it failed to compile, use a very basic error shader.
    if (!success) {
        SetCode(R"(
            Shader {
                Pass {
                    vec4 vertex(VertexData v) {
                        return MATRIX_PROJ*MATRIX_VIEW*MATRIX_MODEL*vec4(v.Position, 1.0f);
                    }

                    color fragment() {
                        return color(0, 0, 0, 1);
                    }
                }
            }
        )");
        Compile();
    }

    return success;
}

void Starsurge::Shader::Use(unsigned int pass) {
    if (this->needs_recompiling) {
        Compile();
    }
    glUseProgram(passes[pass].program);
}
