#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include "../../include/Engine.h"
using namespace Starsurge;


class BasicGame : public Starsurge::Game {
public:
    std::string message;
    BasicGame() : Game("Basic Game") { }
    ~BasicGame() { }
protected:
    float lastX, lastY;
    float yaw, pitch;
    float fov;
    Vector3 front;

    static void OnEscape(int key, int action, int mods, void * data) {
        BasicGame * game = reinterpret_cast<BasicGame*>(data);
        game->CloseGame();
    }

    static void OnMouseMove(double x, double y, void * data) {
        BasicGame * game = reinterpret_cast<BasicGame*>(data);
        if (game->lastX < 0 || game->lastY < 0) {
            game->lastX = x;
            game->lastY = y;
        }

        float xoffset = x - game->lastX;
        float yoffset = game->lastY - y;
        game->lastX = x;
        game->lastY = y;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        game->yaw += xoffset;
        game->pitch += yoffset;

        game->pitch = Clamp(game->pitch, -89.0f, 89.0f);

        Vector3 dir = Vector3(0,0,0);
        dir.x = std::cos(Radians(game->yaw))*std::cos(Radians(game->pitch));
        dir.y = std::sin(Radians(game->pitch));
        dir.z = std::sin(Radians(game->yaw))*std::cos(Radians(game->pitch));
        game->front = Vector3::Normalize(dir);
    }

    static void OnScroll(double xoffset, double yoffset, void * data) {
        BasicGame * game = reinterpret_cast<BasicGame*>(data);
        game->fov = Clamp(game->fov - (float)yoffset, 1.0f, 90.0f);
        game->camera->FindComponent<Camera>()->SetPerspective(game->fov, 0.1, 100);
    }

    void OnInitialize() {
        fov = 45;
        lastX = -1;
        lastY = -1;
        yaw = -90.0f;
        pitch = 0;
        Input::Inst().LockCursor();
        Input::Inst().KeyCallback(KEY_ESCAPE, OnEscape, this);
        Input::Inst().MousePosCallback(OnMouseMove, this);
        Input::Inst().ScrollCallback(OnScroll, this);
        if (!AssetManager::Inst().Load("tests/basic/container.jpg")) {
            Error("Failed to load container.jpg");
            return;
        }

        Scene::Inst().SetBgColor(Color(51,76,76,255));

        camera = new Entity("Camera");
        camera->AddComponent<Transform>(new Transform(Vector3(0,0,3), Vector3(0,0,0), Vector3(1,1,1)));
        camera->AddComponent<Camera>(new Camera(fov, 0.1, 100));
        Scene::Inst().AddEntity(camera);
        Scene::Inst().SetActiveCamera(camera);
        camera->FindComponent<Camera>()->LookAt(Vector3(0,0,0));
        front = camera->FindComponent<Camera>()->Forwards();

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
        const float cameraSpeed = 2.5f * Timer::Inst().DeltaTime();
        Transform * camTransform = camera->FindComponent<Transform>();
        Vector3 up = camera->FindComponent<Camera>()->Up();
        if (Input::Inst().Key(KEY_W) == INPUT_PRESS)
            camTransform->Position += cameraSpeed * front;
        if (Input::Inst().Key(KEY_S) == INPUT_PRESS)
            camTransform->Position -= cameraSpeed * front;
        if (Input::Inst().Key(KEY_A) == INPUT_PRESS)
            camTransform->Position -= cameraSpeed * Vector3::Normalize(Vector3::CrossProduct(front, up));
        if (Input::Inst().Key(KEY_D) == INPUT_PRESS)
            camTransform->Position += cameraSpeed * Vector3::Normalize(Vector3::CrossProduct(front, up));
        camera->FindComponent<Camera>()->LookAt(camTransform->Position + front);


        // Uniform * uniform_color = cube_mat.GetUniform("fragColor");
        // uniform_color->SetData(Color(0,255.0f*(sin(glfwGetTime()) / 2.0 + 0.5f),0,255));
    }
private:
    Entity * camera;
    float angle;
    std::vector<Entity*> cubes;
    Mesh * cube_mesh;
    Material cube_mat;
};

void main() {
    std::cout << Starsurge::GetVersion() << std::endl;

    BasicGame game;
    game.Run();
}
