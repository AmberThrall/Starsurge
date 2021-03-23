#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/GameSettings.h"
#include "../include/Timer.h"
#include "../include/RNG.h"
#include "../include/Input.h"
#include "../include/Engine.h"
#include "../include/stb_image.h"

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Starsurge::Game::Game(std::string t_gamename) : gamename (t_gamename) {
}

Starsurge::Game::~Game() {

}

void Starsurge::Game::CloseGame() {
    glfwDestroyWindow(this->gameWindow);
}

void Starsurge::Game::Run() {
    // Start the clock.
    Timer::Inst().Start();

    // Seed the RNG engine.
    RNG::Inst().Seed(Timer::Inst().EpochTime());

    // Set flags.
    stbi_set_flip_vertically_on_load(true);
    Starsurge::Log("Launching GLFW Window...");

    // Initialize OpenGL version 3.3
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); FOR APPLE

    // Create our window
    this->gameWindow = glfwCreateWindow(800, 600, this->gamename.c_str(), NULL, NULL);
    if (this->gameWindow == NULL) {
        Starsurge::FatalError("Failed to create game window.");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(this->gameWindow);
    glfwSetFramebufferSizeCallback(this->gameWindow, framebuffer_size_callback);
    GameSettings::Inst().gameWindow = this->gameWindow;
    Input::Inst().Setup(this->gameWindow);

    // Initailize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Starsurge::FatalError("Failed to initialize GLAD.");
        return;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO & imguiIO = ImGui::GetIO();
    imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    ImGui_ImplGlfw_InitForOpenGL(this->gameWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Load builtin data.
    AssetManager::Inst().LoadBuiltins();

    OnInitialize();
    GameLoop();
}

void Starsurge::Game::GameLoop() {
    while (!glfwWindowShouldClose(this->gameWindow)) { // Run the game loop until the game is ready to close.
        // Poll input events
        glfwPollEvents();

        // Recalculate the delta time.
        Timer::Inst().UpdateDeltaTime();

        // Updatte all components.
        std::vector<Entity*> allEntities = Scene::Inst().GetAllEntities();
        for (unsigned int i = 0; i < allEntities.size(); ++i) {
            allEntities[i]->Update();
        }

        // Update Dear ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        OnUpdate();

        // Begin Rendering
        ImGui::Render();

        // Clear color for window.
        Color clearColor = Scene::Inst().GetBgColor().ToOpenGLFormat();
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        OnRender();

        // Render Dear imgui last so it is on top.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(this->gameWindow);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void Starsurge::Game::OnRender() {
    // Iterate through each entity with a MeshRenderer
    std::vector<Entity*> meshEntities = Scene::Inst().FindEntitiesWithComponent<MeshRenderer>();
    for (unsigned int i = 0; i < meshEntities.size(); ++i) {
        MeshRenderer * component = meshEntities[i]->FindComponent<MeshRenderer>();
        if (component != NULL) {
            component->Render();
        }
    }
}
