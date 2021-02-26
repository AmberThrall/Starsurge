#pragma once

namespace Starsurge {
    class Entity {
    public:
        Entity(std::string t_name);
        ~Entity();
    private:
        std::string name;
        bool enabled;
        // transform
    };
}
