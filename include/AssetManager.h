#pragma once
#include <map>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Logging.h"

namespace Starsurge {
    class AssetManager {
    public:
        static AssetManager& Inst();

        bool Load(std::string path);
        bool LoadBuiltins();

        Texture * GetTexture(std::string path);
        Mesh * GetMesh(std::string path);
        Shader * GetShader(std::string path);
    private:
        AssetManager() {};

        std::map<std::string, Texture*> textures;
        std::map<std::string, Mesh*> meshes;
        std::map<std::string, Shader*> shaders;
    };
}
