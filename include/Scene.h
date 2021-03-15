#pragma once
#include <vector>
#include "Color.h"
#include "Entity.h"

namespace Starsurge {
    class Scene {
    public:
        static Scene& Inst();

        void SetBgColor(Color t_bgcolor);
        Color GetBgColor();
        void SetActiveCamera(Entity * t_entity);
        Entity * GetActiveCamera();
        void AddEntity(Entity * entity);

        std::vector<Entity*> GetAllEntities();
        std::vector<Entity*> FindEntities(std::string name);
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
        Scene();
        ~Scene();
        Color bgcolor;
        std::vector<Entity*> entities;
        Entity* activeCamera;
    };
}
