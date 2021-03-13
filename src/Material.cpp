#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Material.h"
#include <stdexcept>

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


Starsurge::Uniform * Starsurge::Material::GetUniform(std::string name) {
    return &this->data[name];
}

void Starsurge::Material::Apply(unsigned int passno) {
    this->shader->Use(passno);
    for (auto it = this->data.begin(); it != this->data.end(); ++it) {
        std::string name = it->first;
        Uniform uniform = it->second;

        int loc = glGetUniformLocation(shader->GetProgram(passno), name.c_str());
        if (uniform.GetType() == "bool") {
            glUniform1i(loc, (int)uniform.GetData<bool>());
        }
        else if (uniform.GetType() == "int") {
            glUniform1i(loc, uniform.GetData<int>());
        }
        else if (uniform.GetType() == "float") {
            glUniform1f(loc, uniform.GetData<int>());
        }
        else if (uniform.GetType() == "vec2") {
            Vector2 data = uniform.GetData<Vector2>();
            glUniform2f(loc, data[0], data[1]);
        }
        else if (uniform.GetType() == "vec3") {
            Vector3 data = uniform.GetData<Vector3>();
            glUniform3f(loc, data[0], data[1], data[2]);
        }
        else if (uniform.GetType() == "vec4") {
            Vector4 data = uniform.GetData<Vector4>();
            glUniform4f(loc, data[0], data[1], data[2], data[3]);
        }
        else if (uniform.GetType() == "color") {
            Color data = uniform.GetData<Color>();
            Color glslData = data.ToOpenGLFormat();
            glUniform4f(loc, glslData[0], glslData[1], glslData[2], glslData[3]);
        }
        else if (uniform.GetType() == "ivec2" || uniform.GetType() == "bvec2") {
            Vector2 data = uniform.GetData<Vector2>();
            glUniform2i(loc, (int)std::round(data[0]), (int)std::round(data[1]));
        }
        else if (uniform.GetType() == "ivec3" || uniform.GetType() == "bvec3") {
            Vector3 data = uniform.GetData<Vector3>();
            glUniform3i(loc, (int)std::round(data[0]), (int)std::round(data[1]), (int)std::round(data[2]));
        }
        else if (uniform.GetType() == "ivec4" || uniform.GetType() == "bvec4") {
            Vector4 data = uniform.GetData<Vector4>();
            glUniform4i(loc, (int)std::round(data[0]), (int)std::round(data[1]), (int)std::round(data[2]), (int)std::round(data[3]));
        }
    }
}

void Starsurge::Material::SetupData() {
    this->data.clear();
    std::vector<Uniform> uniforms = shader->GetUniforms();
    for (unsigned int i = 0; i < uniforms.size(); ++i) {
        data[uniforms[i].GetName()] = uniforms[i];
    }
}
