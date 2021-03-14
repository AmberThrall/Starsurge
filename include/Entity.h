#pragma once
#include <vector>
#include "Vector.h"
#include "Matrix.h"
#include "Component.h"

namespace Starsurge {
    class Entity {
    public:
        Entity(std::string t_name);
        ~Entity();

        std::string GetName();
        void Rename(std::string t_name);

        void Toggle();
        bool IsEnabled();
        template<typename T>
        void AddComponent(T * component) {
            if (FindComponent<T>() != NULL) {
                Error(std::string("Tried to add multiple ")+std::string(typeid(T).name())+" components to one entity.");
            }
            else {
                component->owner = this;
                this->components.push_back(component);
            }
        }

        template<typename T>
        T * FindComponent() {
            for (unsigned int i = 0; i < this->components.size(); ++i) {
                if (this->components[i]->GetId() == typeid(T).name()) {
                    return (T*)this->components[i];
                }
            }
            return NULL;
        }
    private:
        std::string name;
        bool enabled;
        std::vector<Component*> components;
    };
}
