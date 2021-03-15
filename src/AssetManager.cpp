#include "../include/AssetManager.h"
#include "../include/stb_image.h"

Starsurge::AssetManager& Starsurge::AssetManager::Inst() {
    static AssetManager instance;
    return instance;
}

Starsurge::Texture * Starsurge::AssetManager::GetTexture(std::string path) {
    if (textures.find(path) == textures.end()) { return NULL; }
    return textures[path];
}

Starsurge::Mesh * Starsurge::AssetManager::GetMesh(std::string path) {
    if (meshes.find(path) == meshes.end()) { return NULL; }
    return meshes[path];
}

Starsurge::Shader * Starsurge::AssetManager::GetShader(std::string path) {
    if (shaders.find(path) == shaders.end()) { return NULL; }
    return shaders[path];
}

bool Starsurge::AssetManager::Load(std::string path) {
    if (textures.find(path) != textures.end()) {
        return true;
    }

    int width, height, nchannels;
    unsigned char * data = stbi_load(path.c_str(), &width, &height, &nchannels, 0);
    if (data != NULL) {
        std::vector<Color> converted;
        for (int i = 0; i < width*height*nchannels; i += nchannels) {
            if (nchannels == 1) {
                converted.push_back(Color(float(data[i]), float(data[i]), float(data[i]), 255));
            }
            else if (nchannels == 2) {
                converted.push_back(Color(float(data[i]), float(data[i]), float(data[i]), float(data[i+1])));
            }
            else if (nchannels == 3) {
                converted.push_back(Color(float(data[i]), float(data[i+1]), float(data[i+2]), 255));
            }
            else if (nchannels == 4) {
                converted.push_back(Color(float(data[i]), float(data[i+1]), float(data[i+2]), float(data[i+3])));
            }
        }

        textures[path] = new Texture(converted, width, height);
        stbi_image_free(data);
        return true;
    }

    return false;
}

bool Starsurge::AssetManager::LoadBuiltins() {
    meshes["Builtin/Triangle"] = Mesh::Triangle(Vector3(0,0,0), Vector3(0,1,0), Vector3(1,1,0));
    meshes["Builtin/Quad"] = Mesh::Quad(Vector3(0,0,0), Vector3(0,1,0), Vector3(1,1,0), Vector3(1,0,0));
    meshes["Builtin/Cube"] = Mesh::Cube();
    shaders["Builtin/BasicShader"] = &Shaders::BasicShader;
    shaders["Builtin/BasicLighting"] = &Shaders::BasicLighting;
    return true;
}
