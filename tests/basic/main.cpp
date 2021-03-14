#include "../../include/Engine.h"
using namespace Starsurge;

class BasicGame : public Starsurge::Game {
public:
    BasicGame() : Game("Basic Game") { }
    ~BasicGame() { }
protected:
    void OnInitialize() {
        Matrix4 transform = Matrix4::RotateZ(Radians(45))*Matrix4::Scale(Vector3(0.5));
        if (!AssetManager::Inst().Load("tests/basic/container.jpg")) {
            Error("Failed to load container.jpg");
            return;
        }

        Scene * scene = new Scene();
        SetScene(scene);
        scene->SetBgColor(Color(51,76,76,255));

        Entity * square = new Entity("Square");
        square_mesh = Mesh::Quad(Vector3(0.5, 0.5, 0.0), Vector3(0.5, -0.5, 0), Vector3(-0.5, -0.5, 0.0), Vector3(-0.5, 0.5, 0.0));
        square_mat = Material(&Shaders::BasicShader);
        square->AddComponent<MeshRenderer>(new MeshRenderer(&square_mesh, &square_mat));

        scene->AddEntity(square);

        Uniform * uniform_texture0 = square_mat.GetUniform("Texture");
        uniform_texture0->SetData(AssetManager::Inst().GetTexture("tests/basic/container.jpg"));

        Uniform * uniform_transform = square_mat.GetUniform("transform");
        uniform_transform->SetData(transform);
    }

    void OnUpdate() {
        Uniform * uniform_color = square_mat.GetUniform("fragColor");
        uniform_color->SetData(Color(0,255.0f*(sin(glfwGetTime()) / 2.0 + 0.5f),0,255));
    }
private:
    Mesh square_mesh;
    Material square_mat;
};

void writeCode(std::string data) {
    int minLevel = data.length();
    // std::conditional_t<(N==2 || N==3 || N==4), swizzle<1,0>, Empty> yx;
    std::string code = "std::conditional_t<";
    if (data.find("z") != std::string::npos && minLevel == 2)
        minLevel = 3;
    if (data.find("w") != std::string::npos)
        minLevel = 4;
    if (minLevel == 2) {
        code += "(N==2 || N==3 || N==4)";
    }
    if (minLevel == 3) {
        code += "(N==3 || N==4)";
    }
    if (minLevel == 4) {
        code += "(N==4)";
    }
    code += ", swizzle<";
    for (unsigned int i = 0; i < data.length(); i++) {
        if (i > 0) code += ", ";
        if (data[i] == 'x') { code += "0"; }
        if (data[i] == 'y') { code += "1"; }
        if (data[i] == 'z') { code += "2"; }
        if (data[i] == 'w') { code += "3"; }
    }

    code += ">, Empty> "+data;
    std::string dataColor = "";
    for (unsigned int i = 0; i < data.length(); i++) {
        if (data[i] == 'x') { dataColor += "r"; }
        if (data[i] == 'y') { dataColor += "g"; }
        if (data[i] == 'z') { dataColor += "b"; }
        if (data[i] == 'w') { dataColor += "a"; }
    }
    code += ", "+dataColor+";";

    std::cout << code.c_str() << std::endl;
    return;
}

void generate(std::string chars, int n) {
    if (n == 2) {
        for (size_t i = 0; i < chars.length(); ++i) {
            for (size_t j = 0; j < chars.length(); ++j) {
                std::string combo = "";
                combo += chars[i];
                combo += chars[j];
                writeCode(combo);
            }
        }
    }
    if (n == 3) {
        for (size_t i = 0; i < chars.length(); ++i) {
            for (size_t j = 0; j < chars.length(); ++j) {
                for (size_t h = 0; h < chars.length(); ++h) {
                    std::string combo = "";
                    combo += chars[i];
                    combo += chars[j];
                    combo += chars[h];
                    writeCode(combo);
                }
            }
        }
    }
    if (n == 4) {
        for (size_t i = 0; i < chars.length(); ++i) {
            for (size_t j = 0; j < chars.length(); ++j) {
                for (size_t h = 0; h < chars.length(); ++h) {
                    for (size_t k = 0; k < chars.length(); ++k) {
                        std::string combo = "";
                        combo += chars[i];
                        combo += chars[j];
                        combo += chars[h];
                        combo += chars[k];
                        writeCode(combo);
                    }
                }
            }
        }
    }
}

void main() {
    // generate("xyzw", 2);
    // generate("xyzw", 3);
    // generate("xyzw", 4);
    // return;

    std::cout << Starsurge::GetVersion() << std::endl;

    BasicGame game;
    game.Run();
}
