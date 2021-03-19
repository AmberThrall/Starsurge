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
        camera->AddComponent<Transform>(new Transform(Vector3(5,5,5), EulerAngles(0,0,0), Vector3(1,1,1)));
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
        cubeMaterial->GetUniform(0, "material.shininess")->SetData(0);
    }

    void OnUpdate() {
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
    Entity * cube;
    Grid * grid;
    Entity * selectedEntity;
    Entity * camera;
};

void main() {
    Editor editor;
    editor.Run();
}
