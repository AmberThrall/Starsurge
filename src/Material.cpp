#include "../include/Material.h"

Starsurge::Material::Material() {
    this->shader = &Shaders::BasicShader;
}

Starsurge::Material::Material(Shader * t_shader) : shader(t_shader) {
    this->shader->Compile();
}

Starsurge::Shader * Starsurge::Material::GetShader() {
    return this->shader;
}
