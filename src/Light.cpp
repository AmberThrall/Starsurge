#include "../include/Light.h"
#include "../include/Entity.h"
#include "../include/Transform.h"
#include "../include/Utils.h"

Starsurge::Light::Light(LightType t_type) : Component(typeid(Light).name()) {
    this->Type = t_type;
    this->Ambient = Color(0.2*255);
    this->Diffuse = Color(128);
    this->Specular = Colors::WHITE;
    this->PointLight.Constant = 1.0;
    this->PointLight.Linear = 0.09;
    this->PointLight.Quadratic = 0.032;
    this->DirectionalLight.Direction = Vector3(1,0,0);
    this->Spotlight.Direction = Vector3(1,0,0);
    this->Spotlight.InnerCone = 12.5;
    this->Spotlight.OuterCone = 17.5;
}

void Starsurge::Light::Apply(Material * mat, unsigned int passno, unsigned int id) {
    Uniform * uniform_type = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].type");
    Uniform * uniform_pos = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].position");
    Uniform * uniform_dir = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].direction");
    Uniform * uniform_ambient = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].ambient");
    Uniform * uniform_diffuse = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].diffuse");
    Uniform * uniform_specular = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].specular");
    Uniform * uniform_constant = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].constant");
    Uniform * uniform_linear = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].linear");
    Uniform * uniform_quadratic = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].quadratic");
    Uniform * uniform_inner_radius = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].innerRadius");
    Uniform * uniform_outer_radius = mat->GetUniform(0, "LIGHTS["+std::to_string(id)+"].outerRadius");

    if (uniform_type) { uniform_type->SetData((int)this->Type); }
    if (uniform_ambient) { uniform_ambient->SetData(this->Ambient); }
    if (uniform_diffuse) { uniform_diffuse->SetData(this->Diffuse); }
    if (uniform_specular) { uniform_specular->SetData(this->Specular); }

    if (this->Type == POINT_LIGHT) {
        Transform * transform = GetOwner()->FindComponent<Transform>();
        if (uniform_pos && transform) { uniform_pos->SetData(transform->Position); }
        if (uniform_constant) { uniform_constant->SetData(this->PointLight.Constant); }
        if (uniform_linear) { uniform_linear->SetData(this->PointLight.Linear); }
        if (uniform_quadratic) { uniform_quadratic->SetData(this->PointLight.Quadratic); }
    }

    if (this->Type == DIRECTIONAL_LIGHT) {
        if (uniform_dir) { uniform_dir->SetData(this->DirectionalLight.Direction); }
    }

    if (this->Type == SPOTLIGHT) {
        Transform * transform = GetOwner()->FindComponent<Transform>();
        if (uniform_pos && transform) { uniform_pos->SetData(transform->Position); }
        if (uniform_dir) { uniform_dir->SetData(this->Spotlight.Direction); }
        if (uniform_inner_radius) { uniform_inner_radius->SetData(std::cos(Radians(this->Spotlight.InnerCone))); }
        if (uniform_outer_radius) { uniform_outer_radius->SetData(std::cos(Radians(this->Spotlight.OuterCone))); }
    }
}
