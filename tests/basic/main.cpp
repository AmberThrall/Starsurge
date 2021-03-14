#include "../../include/Engine.h"
using namespace Starsurge;

class BasicGame : public Starsurge::Game {
public:
    BasicGame() : Game("Basic Game") { }
    ~BasicGame() { }
protected:
    void OnInitialize() {
        Vector4 v = Vector4(0, 0, 0, 0);
        v.xy = Vector2(3,4);
        v.xy += Vector2(1,1);
        Log("v == "+v.ToString());
        Log("v.xy + [1,1] = "+(Vector2(1,1)+v.xy).ToString());
        Log("v.xy + v.xy = "+(Vector2(1,1)+v.xy).ToString());
        v.xy *= 3.0;
        Log("v == "+v.ToString());
        Log("v.xy == v.xy? "+std::string(v.xy == v.xy ? "true" : "false"));
        v.xy += v.xy;
        Log("v == "+v.ToString());
        v.xy = v.yx;
        Log("v == "+v.ToString());
        v.xy = Vector2(0,1);
        Log("v == "+v.ToString());
        v.x = 22;
        Log("v == "+v.ToString());
        Log("v.x+2 = "+std::to_string(v.x+2));
        Log("v.x < 25? "+std::string(v.x < 25 ? "true" : "false"));
        //v.xy.x;
        v.y = 2;
        Log("v == "+v.ToString());

        Matrix4 transform = Matrix4::RotateZ(Radians(90))*Matrix4::Scale(Vector3(0.5));
        if (!AssetManager::Inst().Load("tests/basic/awesomeface.png")) {
            Error("Failed to load container.jpg");
            return;
        }

        Scene * scene = new Scene();
        SetScene(scene);
        scene->SetBgColor(Color(51,76,76,255));

        Log("Scene setup.");
        Entity * square = new Entity("Square");
        square_mesh = Mesh::Quad(Vector3(0.5, 0.5, 0.0), Vector3(0.5, -0.5, 0), Vector3(-0.5, -0.5, 0.0), Vector3(-0.5, 0.5, 0.0));
        Log("Square added 1.");
        square_mat = Material(&Shaders::BasicShader);
        Log("Square added 2.");
        square->AddComponent<MeshRenderer>(new MeshRenderer(&square_mesh, &square_mat));
        Log("Square added 3.");

        scene->AddEntity(square);
        Log("Square added.");

        Uniform * uniform_texture0 = square_mat.GetUniform("Texture");
        uniform_texture0->SetData(AssetManager::Inst().GetTexture("tests/basic/awesomeface.png"));

        Uniform * uniform_transform = square_mat.GetUniform("transform");
        uniform_transform->SetData(transform);

        Log("End of OnInitialize()");
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
    // w = swizzleScalar<3>(this);
    std::string code = "swizzle";
    if (data.find("z") != std::string::npos && minLevel == 2)
        minLevel = 3;
    if (data.find("w") != std::string::npos)
        minLevel = 4;
    if (minLevel > 4) {
        return;
    }
    code = data + " = swizzle<";
    for (unsigned int i = 0; i < data.length(); i++) {
        if (i > 0) code += ", ";
        if (data[i] == 'x') { code += "0"; }
        if (data[i] == 'y') { code += "1"; }
        if (data[i] == 'z') { code += "2"; }
        if (data[i] == 'w') { code += "3"; }
    }

    code += ">(this);";
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
    // // return;

    std::cout << Starsurge::GetVersion() << std::endl;

    BasicGame game;
    game.Run();
}
