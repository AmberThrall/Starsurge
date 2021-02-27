#pragma once
#include <vector>
#include "Color.h"
#include "Entity.h"

namespace Starsurge {
    class Scene {
    public:
        Scene();
        ~Scene();

        void SetBgColor(Color t_bgcolor);
        Color GetBgColor();
        void AddEntity(Entity * entity);
        Entity * FindEntity(std::string name);

        template<typename T>
        std::vector<Entity*> FindEntitiesWithComponent() {
            std::vector<Entity*> ret;
            for (unsigned int i = 0; i < this->entities.size(); ++i) {
                T * search = this->entities[i]->FindComponent<T>();
                if (search != NULL) {
                    ret.push_back(this->entities[i]);
                }
            }
            return ret;
        }

    private:
        Color bgcolor;
        std::vector<Entity*> entities;
    };
}
