#include "../include/Component.h"

Starsurge::Component::Component(std::string t_id) : id(t_id), enabled(true) {

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
