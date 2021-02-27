#pragma once
#include <string>

namespace Starsurge {
    class Component {
    public:
        Component(std::string t_id);

        void Toggle();
        bool IsEnabled();
        std::string GetId();
    protected:
        std::string id;
        bool enabled;
    };
}
