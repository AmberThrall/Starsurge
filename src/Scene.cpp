#include "../include/Scene.h"

Starsurge::Scene::Scene() { }
Starsurge::Scene::~Scene() { }

void Starsurge::Scene::SetBgColor(Color t_bgcolor) {
    this->bgcolor = t_bgcolor;
}

Starsurge::Color Starsurge::Scene::GetBgColor() {
    return this->bgcolor;
}

void Starsurge::Scene::AddEntity(Entity * entity) {
    if (FindEntity(entity->GetName()) != NULL) {
        Error("Tried to add multiple entities with the name "+entity->GetName()+".");
    }
    else {
        this->entities.push_back(entity);
    }
}

Starsurge::Entity * Starsurge::Scene::FindEntity(std::string name) {
    for (unsigned int i = 0; i < this->entities.size(); ++i) {
        if (this->entities[i]->GetName() == name) {
            return this->entities[i];
        }
    }
    return NULL;
}
