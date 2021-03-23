#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/AssetManager.h"
#include "../include/GameSettings.h"
#include "../include/Timer.h"
#include "../include/RNG.h"
#include "../include/Input.h"
#include "../include/Engine.h"
#include "../include/stb_image.h"

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    Starsurge::GameSettings::Inst().SetViewport(0, 0, width, height);
}

Starsurge::Game::Game(std::string t_gamename) : gamename (t_gamename) {
}

Starsurge::Game::~Game() {

}

void Starsurge::Game::CloseGame() {
    glfwDestroyWindow(this->gameWindow);
}

Starsurge::Framebuffer * Starsurge::Game::GetFramebuffer() {
    return this->framebuffer;
}

void Starsurge::Game::AddPostProcessingEffect(Shader * shader) {
    this->postProcessingEffects.push_back(shader);
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

    // Setup our framebuffer.
    GameSettings::Inst().SetViewport(0, 0, 800, 600);
    this->viewport = GameSettings::Inst().GetViewport();
    this->framebuffer = new Framebuffer(this->viewport.GetSize().x, this->viewport.GetSize().y);
    this->framebuffer2 = new Framebuffer(this->viewport.GetSize().x, this->viewport.GetSize().y);
    this->basicPPE.SetCode(R"(
        PostProcessingEffect {
            Pass {
                color fragment() {
                    return texture(SCREEN, UV);
                }
            }
        }
    )");
    this->basicPPE.Compile();
    float screenVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &this->screenVAO);
    glGenBuffers(1, &this->screenVBO);
    glBindVertexArray(this->screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), &screenVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Let the user initialize stuff then enter the main loop.
    OnInitialize();
    GameLoop();
}

void Starsurge::Game::GameLoop() {
    while (!glfwWindowShouldClose(this->gameWindow)) { // Run the game loop until the game is ready to close.
        // Update the viewport information.
        Rect newViewport = GameSettings::Inst().GetViewport();
        if (newViewport != viewport) {
            this->viewport = newViewport;
            this->framebuffer->Resize(this->viewport.GetSize().x, this->viewport.GetSize().y);
            this->framebuffer2->Resize(this->viewport.GetSize().x, this->viewport.GetSize().y);
        }

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

        // Call the user's update.
        OnUpdate();

        // Render the scene to the framebuffer.
        this->framebuffer->Bind();
        Color clearColor = Scene::Inst().GetBgColor().ToOpenGLFormat();
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        OnRender();

        // Perform post processing effects.
        int curFB = 1;
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(this->screenVAO);
        for (unsigned int i = 0; i < this->postProcessingEffects.size(); ++i) {
            for (unsigned int j = 0; j < this->postProcessingEffects[i]->NumberOfPasses(); ++j) {
                if (curFB == 1) {
                    this->framebuffer2->Bind();
                    this->framebuffer->GetTexture()->BindTexture(0);
                    curFB = 2;
                }
                else {
                    this->framebuffer->Bind();
                    this->framebuffer2->GetTexture()->BindTexture(0);
                    curFB = 1;
                }
                this->postProcessingEffects[i]->Use(j);
                glUniform1i(glGetUniformLocation(this->postProcessingEffects[i]->GetProgram(j), "SCREEN"), 0);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // Draw the final result.
        if (curFB == 1) {
            this->framebuffer->Unbind();
            this->framebuffer->GetTexture()->BindTexture(0);
        }
        else {
            this->framebuffer2->Unbind();
            this->framebuffer2->GetTexture()->BindTexture(0);
        }
        ImGui::Render();
        this->basicPPE.Use(0);
        glUniform1i(glGetUniformLocation(this->basicPPE.GetProgram(0), "SCREEN"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Render Dear imgui last so it is on top.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(this->gameWindow);

        // Poll input events
        glfwPollEvents();
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
