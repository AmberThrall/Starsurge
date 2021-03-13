#pragma once
#include <map>
#include "Texture.h"
#include "Logging.h"

namespace Starsurge {
    class AssetManager {
    public:
        static AssetManager& Inst();

        bool Load(std::string path);

        Texture * GetTexture(std::string path);
    private:
        AssetManager() {};

        std::map<std::string, Texture*> textures;
    };
}
