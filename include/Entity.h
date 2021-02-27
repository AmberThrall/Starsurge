#pragma once
#include <vector>
#include "Vector.h"
#include "Component.h"

namespace Starsurge {
    class Entity {
    public:
        Entity(std::string t_name);
        ~Entity();

        void Toggle();
        bool IsEnabled();
        std::string GetName();
        template<typename T>
        void AddComponent(T * component) {
            if (FindComponent<T>() != NULL) {
                Error(std::string("Tried to add multiple ")+std::string(typeid(T).name())+" components to one entity.");
            }
            else {
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

        Vector3 position;
        Vector3 rotation;
        Vector3 scaling;

        std::vector<Component*> components;
    };
}
