#pragma once
#include <string>
#include "Logging.h"

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
        virtual void OnUpdate() {};

        std::string id;
        bool enabled;
        Entity * owner;

        friend class Entity;
    };
}
