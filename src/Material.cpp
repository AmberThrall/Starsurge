#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Material.h"
#include <stdexcept>

Starsurge::Material::Material() {
    shader = NULL;
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
    // Textures need to bind before we use our shader.
    unsigned int texNum = 0;
    for (auto it = this->data.begin(); it != this->data.end(); ++it) {
        std::string name = it->first;
        Uniform uniform = it->second;
        if (uniform.GetType() == "sampler2D") {
            Texture * data = uniform.GetData<Texture*>();
            if (data) {
                data->BindTexture(texNum++);
            }
        }
    }

    // Now we activate the shader and use glUniform*
    texNum = 0;
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
            glUniform1f(loc, uniform.GetData<float>());
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
        else if (uniform.GetType() == "sampler2D") {
            Texture * data = uniform.GetData<Texture*>();
            if (data) {
                glUniform1i(loc, texNum++);
            }
        }
        else if (uniform.GetType() == "mat2" || uniform.GetType() == "mat2x2") {
            Matrix2 data = uniform.GetData<Matrix2>();
            glUniformMatrix2fv(loc, 1, GL_TRUE, data.Ptr());
        }
        else if (uniform.GetType() == "mat3" || uniform.GetType() == "mat3x3") {
            Matrix3 data = uniform.GetData<Matrix3>();
            glUniformMatrix3fv(loc, 1, GL_TRUE, data.Ptr());
        }
        else if (uniform.GetType() == "mat4" || uniform.GetType() == "mat4x4") {
            Matrix4 data = uniform.GetData<Matrix4>();
            glUniformMatrix4fv(loc, 1, GL_TRUE, data.Ptr());
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
