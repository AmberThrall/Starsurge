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
        // Initialize variables.
        firstUpdate = true;

        // Window settings
        GameSettings::Inst().ResizeWindow(1600, 900);
        Scene::Inst().SetBgColor(Color(59,59,59));

        // Grid
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
        cube->AddComponent<MeshRenderer>(new MeshRenderer(&yzGrid, cubeMaterial));
        cube->FindComponent<MeshRenderer>()->SetWireframe(true);
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
        for (int i = aspect16by9.size()-1; i >= 0; i--) {
            if (w >= aspect16by9[i].x && h >= aspect16by9[i].y) {
                return aspect16by9[i];
            }
        }
        return Vector2(w, h);
    }

    void Gui() {
        ImGuiStyle & style = ImGui::GetStyle();
        ImGuiStyle styleBackup = style;
        style.WindowRounding = 0;
        style.FrameRounding = 0;
        style.Colors[ImGuiCol_WindowBg].w = 1;
        style.WindowPadding = ImVec2(0,0);
        style.ItemSpacing = ImVec2(0,0);
        ImGui::Begin("Frames", &my_tool_active, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize
                                                | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
                                                | ImGuiWindowFlags_NoTitleBar);
            // Main menu
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

            // Split the frame into 4 parts.
            float gap = 2;
            float w = ImGui::GetWindowContentRegionMax().x-ImGui::GetWindowContentRegionMin().x;
            float h = ImGui::GetWindowContentRegionMax().y-ImGui::GetWindowContentRegionMin().y;
            if (split1Size1 + split1Size2 + gap != w) {
                split1Size1 = BestSceneSize(w-gap, h).x;
                split1Size2 = w - split1Size1 - gap;
            }
            ImGui::Splitter(false, gap, &split1Size1, &split1Size2, 8, 8, w);
            ImGui::BeginChild("1", ImVec2(split1Size1, h), true);
                float w2 = ImGui::GetWindowContentRegionMax().x-ImGui::GetWindowContentRegionMin().x;
                float h2 = ImGui::GetWindowContentRegionMax().y-ImGui::GetWindowContentRegionMin().y;
                if (split2Size1 + split2Size2 + gap != h2) {
                    split2Size1 = BestSceneSize(w2, h2-gap).y;
                    split2Size2 = h2 - split2Size1 - gap;
                }
                ImGui::Splitter(true, gap, &split2Size1, &split2Size2, 8, 8, w2);
                ImGui::BeginChild("TopLeft", ImVec2(w2, split2Size1), true);
                    dockspaceTL = ImGui::GetID("Dockspace Top Left");
                    ImGui::DockSpace(dockspaceTL, ImVec2(split1Size1, split2Size1), ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_NoResize);
                ImGui::EndChild();
                ImGui::BeginChild("BottomLeft", ImVec2(w2, split2Size2), true);
                    dockspaceBL = ImGui::GetID("Dockspace Bottom Left");
                    ImGui::DockSpace(dockspaceBL, ImVec2(split1Size1, split2Size2), ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_NoResize);
                ImGui::EndChild();
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("2", ImVec2(split1Size2, h), true);
                float w3 = ImGui::GetWindowContentRegionMax().x-ImGui::GetWindowContentRegionMin().x;
                float h3 = ImGui::GetWindowContentRegionMax().y-ImGui::GetWindowContentRegionMin().y;
                if (split3Size1 + split3Size2 + gap != h3) {
                    split3Size1 = h3*0.25;
                    split3Size2 = h3 - split3Size1 - gap;
                }
                ImGui::Splitter(true, gap, &split3Size1, &split3Size2, 8, 8, w3);
                ImGui::BeginChild("TopRight", ImVec2(w3, split3Size1), true);
                    dockspaceTR = ImGui::GetID("Dockspace Top Right");
                    ImGui::DockSpace(dockspaceTR, ImVec2(split1Size2, split3Size1), ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_NoResize);
                ImGui::EndChild();
                ImGui::BeginChild("BottomRight", ImVec2(w3, split3Size2), true);
                    dockspaceBR = ImGui::GetID("Dockspace Bottom Right");
                    ImGui::DockSpace(dockspaceBR, ImVec2(split1Size2, split3Size2), ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_NoResize);
                ImGui::EndChild();
            ImGui::EndChild();

            // Move and resize the frame to the whole actual window.
            ImGui::SetWindowPos(ImVec2(0,0), true);
            ImGui::SetWindowSize(ImVec2((int)windowSize.x, (int)windowSize.y), true);
        ImGui::End();

        // Scene
        if (firstUpdate)
            ImGui::SetNextWindowDockID(dockspaceTL);
        ImGui::Begin("Scene");
            w = ImGui::GetWindowContentRegionMax().x-ImGui::GetWindowContentRegionMin().x;
            h = ImGui::GetWindowContentRegionMax().y-ImGui::GetWindowContentRegionMin().y;
            GameSettings::Inst().SetViewport(0,0,w,h);
            camera->FindComponent<Camera>()->SetPerspective(60, w / h, 0.1, 100);
            ImGui::Image((ImTextureID)GetFramebuffer()->GetTexture()->GetTexture(), ImVec2(w,h), ImVec2(0,1), ImVec2(1,0));
        ImGui::End();

        // Assets
        style = styleBackup;
        if (firstUpdate)
            ImGui::SetNextWindowDockID(dockspaceBL);
        ImGui::Begin("Assets");
            ImGui::Text("Assets goes here.");
        ImGui::End();

        // Hierarchy
        if (firstUpdate)
            ImGui::SetNextWindowDockID(dockspaceTR);
        ImGui::Begin("Hierarchy");
            ImGui::Text("Hierarchy goes here.");
        ImGui::End();

        // Inspector
        if (firstUpdate)
            ImGui::SetNextWindowDockID(dockspaceBR);
        ImGui::Begin("Inspector");
            ImGui::Text("Inspector goes here.");
        ImGui::End();
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

        firstUpdate = false;
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
    Mesh yzGrid;

    // Gui Stuff
    bool firstUpdate;
    float split1Size1;
    float split1Size2;
    float split2Size1;
    float split2Size2;
    float split3Size1;
    float split3Size2;
    ImGuiID dockspaceTL, dockspaceTR, dockspaceBL, dockspaceBR;
};

void main() {
    Editor editor;
    editor.Run();
}
