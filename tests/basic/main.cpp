#include "../../include/Engine.h"
using namespace Starsurge;

class BasicGame : public Starsurge::Game {
public:
    BasicGame() : Game("Basic Game") { }
    ~BasicGame() { }
protected:
    void OnInitialize() {
        if (!AssetManager::Inst().Load("tests/basic/container.jpg")) {
            Error("Failed to load container.jpg");
            return;
        }

        Scene::Inst().SetBgColor(Color(51,76,76,255));

        camera = new Entity("Camera");
        camera->AddComponent<Transform>(new Transform(Vector3(0,0,3), Vector3(0,0,0), Vector3(1,1,1)));
        camera->AddComponent<Camera>(new Camera(Radians(45), 0.1, 100));
        Scene::Inst().AddEntity(camera);
        Scene::Inst().SetActiveCamera(camera);
        camera->FindComponent<Camera>()->LookAt(Vector3(0,0,0));

        std::vector<Vector3> cubePositions = {
            Vector3( 0.0f,  0.0f,  0.0f),
            Vector3( 2.0f,  5.0f, -15.0f),
            Vector3(-1.5f, -2.2f, -2.5f),
            Vector3(-3.8f, -2.0f, -12.3f),
            Vector3( 2.4f, -0.4f, -3.5f),
            Vector3(-1.7f,  3.0f, -7.5f),
            Vector3( 1.3f, -2.0f, -2.5f),
            Vector3( 1.5f,  2.0f, -2.5f),
            Vector3( 1.5f,  0.2f, -1.5f),
            Vector3(-1.3f,  1.0f, -1.5f)
        };

        Log("Building cubes.");
        cube_mesh = AssetManager::Inst().GetMesh("Builtin/Cube");
        cube_mat = Material(AssetManager::Inst().GetShader("Builtin/BasicShader"));
        for (size_t i = 0; i < cubePositions.size(); ++i) {
            Entity * cube = new Entity("Cube"+std::to_string(i+1));
            cube->AddComponent<Transform>(new Transform(cubePositions[i], Vector3(Radians(10.0f*i),Radians(5.0f*i),Radians(-2.0f*i)), Vector3(1,1,1)));
            cube->AddComponent<MeshRenderer>(new MeshRenderer(cube_mesh, &cube_mat));
            Scene::Inst().AddEntity(cube);
            cubes.push_back(cube);
        }

        Log("Setting uniforms.");
        Uniform * uniform_texture0 = cube_mat.GetUniform("Texture");
        uniform_texture0->SetData(AssetManager::Inst().GetTexture("tests/basic/container.jpg"));
    }

    void OnUpdate() {
        // Uniform * uniform_color = cube_mat.GetUniform("fragColor");
        // uniform_color->SetData(Color(0,255.0f*(sin(glfwGetTime()) / 2.0 + 0.5f),0,255));

        Transform * camTransform = camera->FindComponent<Transform>();
        camTransform->Position = Vector3(sin(glfwGetTime()) * 10.0f, 0, cos(glfwGetTime()) * 10.0f);
    }
private:
    Entity * camera;
    float angle;
    std::vector<Entity*> cubes;
    Mesh * cube_mesh;
    Material cube_mat;
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
