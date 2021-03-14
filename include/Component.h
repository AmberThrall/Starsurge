#pragma once
#include <string>

namespace Starsurge {
    class Entity;

    class Component {
    public:
        Component(std::string t_id);

        Entity * GetOwner();

        void Toggle();
        bool IsEnabled();
        std::string GetId();
    protected:
        std::string id;
        bool enabled;
        Entity * owner;

        friend class Entity;
    };
}
