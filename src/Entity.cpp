#include "../include/Entity.h"
#include "../include/Logging.h"

Starsurge::Entity::Entity(std::string t_name) : name(t_name), enabled(true) {

}

Starsurge::Entity::~Entity() {
    //TODO: Cleanup
}

void Starsurge::Entity::Toggle() {
    this->enabled = !this->enabled;
}

bool Starsurge::Entity::IsEnabled() {
    return this->enabled;
}

std::string Starsurge::Entity::GetName() {
    return this->name;
}
