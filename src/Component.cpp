#include "../include/Component.h"
#include "../include/Entity.h"

Starsurge::Component::Component(std::string t_id) : id(t_id), enabled(true) {

}

Starsurge::Entity * Starsurge::Component::GetOwner() {
    return this->owner;
}

void Starsurge::Component::Toggle() {
    this->enabled = !this->enabled;
}
bool Starsurge::Component::IsEnabled() {
    return this->enabled;
}
std::string Starsurge::Component::GetId() {
    return this->id;
}
