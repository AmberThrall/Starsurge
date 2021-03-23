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

    Vector2 BestSceneSize(int w, int h) {
        std::vector<Vector2> aspect16by9 = {
            {1024,576}, {1152,648}, {1280,720}, {1366,768}, {1600,900}, {1920,1080}, {2560,1440}, {3840,2160}
        };
        Log("In: ("+std::to_string(w)+","+std::to_string(h)+")");
        for (int i = aspect16by9.size()-1; i >= 0; i--) {
            if (w >= aspect16by9[i].x && h >= aspect16by9[i].y) {
                Log("Got: "+aspect16by9[i].ToString());
                return aspect16by9[i];
            }
        }

        Log("Nothing found.");
        return Vector2(w, h);
    }

    void Gui() {
        ImGuiStyle & style = ImGui::GetStyle();
        ImGuiStyle styleBackup = style;
        style.WindowRounding = 0;
        style.FrameRounding = 0;
        style.Colors[ImGuiCol_WindowBg].w = 1;
        style.WindowPadding = ImVec2(0,0);
        ImGui::Begin("Frames", &my_tool_active, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize
                                                | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
                                                | ImGuiWindowFlags_NoTitleBar);
            float menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("New Project", "Ctrl+N")) {
                        Log("New project");
                    }
                    if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                        Log("Open project");
                    }
                    if (ImGui::MenuItem("Save...", "Ctrl+O")) {
                        Log("Save project");
                    }
                    if (ImGui::MenuItem("Exit")) {
                        CloseGame();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            Vector2 windowSize = GameSettings::Inst().GetWindowSize();
            // Splitter 1
            float gap = 8;
            float splitter1Height = windowSize.y-menuBarHeight;
            if (split1Size1 <= 0) {
                split1Size1 = BestSceneSize(windowSize.x-gap, splitter1Height).x;
                split1Size2 = windowSize.x - split1Size1 - gap;
            }
            if (split1Size1 >= windowSize.x - gap) {
                split1Size1 = BestSceneSize(windowSize.x-gap, splitter1Height).x;
                split1Size2 = windowSize.x - split1Size1 - gap;
            }
            ImGui::Splitter(false, gap, &split1Size1, &split1Size2, gap, gap, splitter1Height);
            ImGui::BeginChild("1", ImVec2(split1Size1, splitter1Height), true);
                if (split2Size1 <= 0) {
                    split2Size1 = BestSceneSize(split1Size1, splitter1Height-gap).y;
                    split2Size2 = splitter1Height - split2Size1 - gap;
                }
                if (split2Size1 >= splitter1Height - gap) {
                    split2Size1 = BestSceneSize(split1Size1, splitter1Height-gap).y;
                    split2Size2 = splitter1Height - split2Size1 - gap;
                }
                ImGui::Splitter(true, gap, &split2Size1, &split2Size2, gap, gap, split1Size1);
                ImGui::BeginChild("TopLeft", ImVec2(split1Size1, split2Size1), true);
                    ImGui::Text("Scene.");
                ImGui::EndChild();
                ImGui::BeginChild("BottomLeft", ImVec2(split1Size1, split2Size2), true);
                    ImGui::Text("Assets.");
                ImGui::EndChild();
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("2", ImVec2(split1Size2, splitter1Height), true);
                if (split3Size1 <= 0) {
                    split3Size1 = splitter1Height*0.25;
                    split3Size2 = splitter1Height - split3Size1 - gap;
                }
                if (split3Size1 >= splitter1Height - gap) {
                    split3Size1 = splitter1Height*0.25;
                    split3Size2 = splitter1Height - split3Size1 - gap;
                }
                ImGui::Splitter(true, gap, &split3Size1, &split3Size2, gap, gap, split1Size2);
                ImGui::BeginChild("TopRight", ImVec2(split1Size2, split3Size1), true);
                    ImGui::Text("Hierarchy.");
                ImGui::EndChild();
                ImGui::BeginChild("BottomRight", ImVec2(split1Size2, split3Size2), true);
                    ImGui::Text("Inspector.");
                ImGui::EndChild();
            ImGui::EndChild();

            // Move and resize the frame to the whole actual window.
            ImGui::SetWindowPos(ImVec2(0,0), true);
            ImGui::SetWindowSize(ImVec2((int)windowSize.x, (int)windowSize.y), true);
        ImGui::End();
        style = styleBackup;

    }

    void OnUpdate() {
        Gui();

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

    // Gui Stuff
    float split1Size1;
    float split1Size2;
    float split2Size1;
    float split2Size2;
    float split3Size1;
    float split3Size2;
};

void main() {
    Editor editor;
    editor.Run();
}
