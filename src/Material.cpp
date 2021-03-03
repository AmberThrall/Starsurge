#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Material.h"
#include <stdexcept>

Starsurge::MaterialData::MaterialData(std::string t_name, std::string t_type) : name(t_name), type(t_type) {

}

std::string Starsurge::MaterialData::GetName() {
    return this->name;
}

std::string Starsurge::MaterialData::GetType() {
    return this->type;
}

void Starsurge::MaterialData::SetData(bool val) {
    if (GetType() != "bool") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type bool.");
    }
    this->data = val;
}

void Starsurge::MaterialData::SetData(int val) {
    if (GetType() != "int") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type int.");
    }
    this->data = val;
}

void Starsurge::MaterialData::SetData(unsigned int val) {
    if (GetType() != "uint") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type uint.");
    }
    this->data = val;
}

void Starsurge::MaterialData::SetData(float val) {
    if (GetType() != "float") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type float.");
    }
    this->data = val;
}

void Starsurge::MaterialData::SetData(double val) {
    if (GetType() != "double") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type double.");
    }
    this->data = val;
}

void Starsurge::MaterialData::SetData(Vector2 val) {
    if (GetType() != "vec2") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type vec2.");
    }
    this->data = val;
}

void Starsurge::MaterialData::SetData(Vector3 val) {
    if (GetType() != "vec3") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type vec3.");
    }
    this->data = val;
}

void Starsurge::MaterialData::SetData(Vector4 val) {
    if (GetType() != "vec4") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type vec4.");
    }
    this->data = val;
}

void Starsurge::MaterialData::SetData(Color val) {
    if (GetType() != "vec4") {
        throw std::runtime_error("Tried to set uniform of type "+GetType()+" to type vec4.");
    }
    Color clearColor = val.ToOpenGLFormat(); //Temporary
    this->data = Vector4(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
}

Starsurge::Material::Material() {
    this->shader = &Shaders::BasicShader;
    SetupData();
}

Starsurge::Material::Material(Shader * t_shader) {
    SetShader(t_shader);
}

void Starsurge::Material::SetShader(Shader * t_shader) {
    this->shader = t_shader;
    this->shader->Compile();
    SetupData();
}

Starsurge::Shader * Starsurge::Material::GetShader() {
    return this->shader;
}


Starsurge::MaterialData * Starsurge::Material::GetUniform(std::string name) {
    return this->data[name];
}

void Starsurge::Material::Apply() {
    this->shader->Use();

    for (auto it = this->data.begin(); it != this->data.end(); ++it) {
        std::string name = it->first;
        MaterialData * mdata = it->second;
        std::string type = mdata->GetType();

        // Get uniform location and apply uniform.
        int uniformLoc = glGetUniformLocation(this->shader->GetProgram(), name.c_str());
        if (type == "vec2") {
            const Vector2 vec2 = mdata->GetData<Vector2>();
            glUniform2f(uniformLoc, vec2[0], vec2[1]);
        }
        if (type == "vec3") {
            const Vector3 vec3 = mdata->GetData<Vector3>();
            glUniform3f(uniformLoc, vec3[0], vec3[1], vec3[2]);
        }
        if (type == "vec4") {
            const Vector4 vec4 = mdata->GetData<Vector4>();
            glUniform4f(uniformLoc, vec4[0], vec4[1], vec4[2], vec4[3]);
        }
    }
}

void Starsurge::Material::SetupData() {
    this->data.clear();
    const std::map<std::string, std::string> uniforms = this->shader->GetUniforms();

    for (auto it = uniforms.begin(); it != uniforms.end(); ++it) {
        std::string name = it->first;
        std::string type = it->second;

        // Create and initialize our material data.
        MaterialData * matdata = new MaterialData(name, type);
        if (type == "bool") { matdata->SetData(false); }
        if (type == "int") { matdata->SetData(0); }
        if (type == "uint") { matdata->SetData(0); }
        if (type == "float") { matdata->SetData(0.0f); }
        if (type == "double") { matdata->SetData(0.0); }
        if (type == "vec2") { matdata->SetData(Vector2(0,0)); }
        if (type == "vec3") { matdata->SetData(Vector3(0,0,0)); }
        if (type == "vec4") { matdata->SetData(Vector4(0,0,0,0)); }

        // Add our data
        this->data[name] = matdata;
    }
}
