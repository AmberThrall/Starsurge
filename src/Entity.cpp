#include "../include/Entity.h"
#include "../include/Logging.h"

Starsurge::Entity::Entity(std::string t_name) : name(t_name), enabled(true) {
}

Starsurge::Entity::~Entity() {
    //TODO: Cleanup
}

std::string Starsurge::Entity::GetName() {
    return this->name;
}

void Starsurge::Entity::Rename(std::string t_name) {
    this->name = t_name;
}

void Starsurge::Entity::Update() {
    if (this->enabled) {
        for (unsigned int i = 0; i < components.size(); ++i) {
            if (components[i]->IsEnabled()) {
                components[i]->OnUpdate();
            }
        }
    }
}

void Starsurge::Entity::Toggle() {
    this->enabled = !this->enabled;
}

bool Starsurge::Entity::IsEnabled() {
    return this->enabled;
}
