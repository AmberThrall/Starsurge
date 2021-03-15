#include "../include/Scene.h"

Starsurge::Scene& Starsurge::Scene::Inst() {
    static Scene instance;
    return instance;
}

Starsurge::Scene::Scene() { this->activeCamera = NULL; }
Starsurge::Scene::~Scene() { }

void Starsurge::Scene::SetBgColor(Color t_bgcolor) {
    this->bgcolor = t_bgcolor;
}
Starsurge::Color Starsurge::Scene::GetBgColor() {
    return this->bgcolor;
}

void Starsurge::Scene::SetActiveCamera(Entity * t_entity) {
    this->activeCamera = t_entity;
}
Starsurge::Entity * Starsurge::Scene::GetActiveCamera() {
    return this->activeCamera;
}

void Starsurge::Scene::AddEntity(Entity * entity) {
    this->entities.push_back(entity);
}

std::vector<Starsurge::Entity*> Starsurge::Scene::GetAllEntities() {
    return this->entities;
}

std::vector<Starsurge::Entity*> Starsurge::Scene::FindEntities(std::string name) {
    std::vector<Entity*> ret;
    for (unsigned int i = 0; i < this->entities.size(); ++i) {
        if (this->entities[i]->GetName() == name) {
            ret.push_back(this->entities[i]);
        }
    }
    return ret;
}
