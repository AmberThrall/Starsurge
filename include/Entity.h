#pragma once
#include "Vector.h"

namespace Starsurge {
    class Entity {
    public:
        Entity(std::string t_name);
        ~Entity();
    private:
        std::string name;
        bool enabled;

        Vector3 position;
        Vector3 rotation;
        Vector3 scaling;
    };
}
