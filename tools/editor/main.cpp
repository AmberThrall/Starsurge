#include "../../include/Engine.h"
#include "Grid.h"
using namespace Starsurge;

class Editor : public Starsurge::Game {
public:
    std::string message;
    Editor() : Game("Starsurge Editor") { }
    ~Editor() { }
protected:
    void OnInitialize() {
        GameSettings::Inst().ResizeWindow(1600, 900);

        Scene::Inst().SetBgColor(Color(59,59,59));

        grid = new Grid(1);

        // Sun
        Entity * sun = new Entity("Sun");
        sun->AddComponent<Light>(new Light(DIRECTIONAL_LIGHT));
        sun->FindComponent<Light>()->DirectionalLight.Direction = Vector3(Radians(-45), 0, 0);
        Scene::Inst().AddEntity(sun);

        // Camera
        camera = new Entity("Editor Camera");
        camera->AddComponent<Transform>(new Transform(Vector3(5,5,5), EulerAngles(0,0,0,YZX), Vector3(1,1,1)));
        camera->AddComponent<Camera>(new Camera(60, 0.1, 100));
        Scene::Inst().AddEntity(camera);
        Scene::Inst().SetActiveCamera(camera);
        camera->FindComponent<Camera>()->LookAt(Vector3(0,0,0));

        // Simple Cube
        cube = new Entity("Cube");
        Material * cubeMaterial = new Material(AssetManager::Inst().GetShader("Builtin/Phong"));
        cube->AddComponent<Transform>(new Transform(Vector3(0,0,0), EulerAngles(0,0,0), Vector3(1,1,1)));
        cube->AddComponent<MeshRenderer>(new MeshRenderer(AssetManager::Inst().GetMesh("Builtin/Cube"), cubeMaterial));
        Scene::Inst().AddEntity(cube);
        selectedEntity = cube;
        cubeMaterial->GetUniform(0, "material.diffuse")->SetData(AssetManager::Inst().GetTexture("Builtin/White"));
        cubeMaterial->GetUniform(0, "material.specular")->SetData(AssetManager::Inst().GetTexture("Builtin/Black"));
        cubeMaterial->GetUniform(0, "material.shininess")->SetData(32);
    }

    void OnUpdate() {
        ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
        ImGui::ColorEdit4("Color", (float*)&my_color);
        ImGui::TextColored(ImVec4(1,1,0,1), "Hello World!");
        ImGui::End();

        if (Input::Inst().MouseButton(MOUSE_RIGHT) == INPUT_PRESS) {
            Input::Inst().HideCursor();
            const Vector2 sensitivity = Vector2(15, 15);

            EulerAngles euler = camera->FindComponent<Transform>()->Rotation.GetEulerAngles();
            Vector2 windowSize = GameSettings::Inst().GetWindowSize();
            Vector2 mouseDelta = Input::Inst().GetCurserPos();

            // Change mouseDelta to live in [-1,1]x[-1,1]
            mouseDelta.x = (mouseDelta.x / (windowSize.x / 2)) - 1;
            mouseDelta.y = (mouseDelta.y / (windowSize.y / 2)) - 1;

            // Scale the input by sensitivity
            euler.y -= mouseDelta.x * sensitivity.x;
            euler.x -= mouseDelta.y * sensitivity.y;

            // Clamp/warp the angles
            euler.y = Wrap(euler.y, -180, 180);
            euler.x = Clamp(euler.x, -89, 89);

            // Set the euler angles
            camera->FindComponent<Transform>()->Rotation.SetEulerAngles(euler);
            Input::Inst().MoveCursor(windowSize.x / 2, windowSize.y / 2);

            // WASD Movement.
            float moveSpeed = 10.0 * (float)Timer::Inst().DeltaTime();
            if (Input::Inst().Key(KEY_LEFT_SHIFT) == INPUT_PRESS || Input::Inst().Key(KEY_RIGHT_SHIFT) == INPUT_PRESS) {
                moveSpeed *= 5;
            }
            Vector3 eye = camera->FindComponent<Transform>()->Position;
            Vector3 forward = camera->FindComponent<Camera>()->Forward();
            Vector3 right = camera->FindComponent<Camera>()->Right();
            if (Input::Inst().Key(KEY_W) == INPUT_PRESS) {
                eye -= moveSpeed*forward;
            }
            if (Input::Inst().Key(KEY_S) == INPUT_PRESS) {
                eye += moveSpeed*forward;
            }
            if (Input::Inst().Key(KEY_A) == INPUT_PRESS) {
                eye -= moveSpeed*right;
            }
            if (Input::Inst().Key(KEY_D) == INPUT_PRESS) {
                eye += moveSpeed*right;
            }
            camera->FindComponent<Transform>()->Position = eye;
        }
        else {
            Input::Inst().ShowCursor();
        }

        const float rotSpeed = 90 * (float)Timer::Inst().DeltaTime();
        EulerAngles euler = cube->FindComponent<Transform>()->Rotation.GetEulerAngles();
        if (Input::Inst().Key(KEY_X) == INPUT_PRESS) {
            euler.x += rotSpeed;
        }
        if (Input::Inst().Key(KEY_Y) == INPUT_PRESS) {
            euler.y += rotSpeed;
        }
        if (Input::Inst().Key(KEY_Z) == INPUT_PRESS) {
            euler.z += rotSpeed;
        }
        cube->FindComponent<Transform>()->Rotation.SetEulerAngles(euler);
    }

    void OnRender() {
        grid->Render();
        Game::OnRender();
    }

private:
    bool my_tool_active;
    ImVec4 my_color;
    Entity * camera;
    Entity * cube;
    Grid * grid;
    Entity * selectedEntity;
};

void main() {
    Editor editor;
    editor.Run();
}
