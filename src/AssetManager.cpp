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
