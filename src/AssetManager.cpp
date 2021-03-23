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
    // Meshes
    meshes["Builtin/Triangle"] = Mesh::Triangle(Vector3(0,0,0), Vector3(0,1,0), Vector3(1,1,0));
    meshes["Builtin/Quad"] = Mesh::Quad(Vector3(-1,-1,0), Vector3(1,-1,0), Vector3(1,1,0), Vector3(-1,1,0));
    meshes["Builtin/Cube"] = Mesh::Cube();

    // Textures
    textures["Builtin/White"] = new Texture(std::vector<Color>({Colors::WHITE}), 1, 1);
    textures["Builtin/Black"] = new Texture(std::vector<Color>({Colors::BLACK}), 1, 1);

    // Shaders
    shaders["Builtin/BasicShader"] = &Shaders::BasicShader;
    shaders["Builtin/Phong"] = &Shaders::Phong;
    shaders["Builtin/Inversion"] = new Shader(R"(
        PostProcessingEffect {
            Pass {
                color fragment() {
                    return vec4(vec3(1.0 - texture(SCREEN, UV)), 1.0);
                }
            }
        }
    )");
    shaders["Builtin/Inversion"]->Compile();
    shaders["Builtin/Grayscale"] = new Shader(R"(
        PostProcessingEffect {
            Pass {
                color fragment() {
                    vec4 c = texture(SCREEN, UV);
                    float average = 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
                    return vec4(average, average, average, 1.0);
                }
            }
        }
    )");
    shaders["Builtin/Grayscale"]->Compile();
    shaders["Builtin/Blur"] = new Shader(R"(
        PostProcessingEffect {
            Pass {
                color fragment() {
                    const float offset = 1.0 / 300.0;
                    vec2 offsets[9] = vec2[](
                        vec2(-offset,  offset), // top-left
                        vec2( 0.0f,    offset), // top-center
                        vec2( offset,  offset), // top-right
                        vec2(-offset,  0.0f),   // center-left
                        vec2( 0.0f,    0.0f),   // center-center
                        vec2( offset,  0.0f),   // center-right
                        vec2(-offset, -offset), // bottom-left
                        vec2( 0.0f,   -offset), // bottom-center
                        vec2( offset, -offset)  // bottom-right
                    );

                    float kernel[9] = float[](
                        1.0 / 16, 2.0 / 16, 1.0 / 16,
                        2.0 / 16, 4.0 / 16, 2.0 / 16,
                        1.0 / 16, 2.0 / 16, 1.0 / 16
                    );

                    vec3 sampleTex[9];
                    for(int i = 0; i < 9; i++) { sampleTex[i] = vec3(texture(SCREEN, UV.st + offsets[i])); }
                    vec3 col = vec3(0.0);
                    for(int i = 0; i < 9; i++) { col += sampleTex[i] * kernel[i]; }
                    return vec4(col, 1.0);
                }
            }
        }
    )");
    shaders["Builtin/Blur"]->Compile();
    return true;
}
