#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Engine.h"

void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Starsurge::Game::Game(std::string t_gamename) : gamename (t_gamename) {
}

Starsurge::Game::~Game() {

}

void Starsurge::Game::SetScene(Scene * t_scene) {
    //TODO: Loading
    this->activeScene = t_scene;
}

void Starsurge::Game::Run() {
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

    // Initailize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Starsurge::FatalError("Failed to initialize GLAD.");
        return;
    }

    OnInitialize();
    GameLoop();
}

void Starsurge::Game::GameLoop() {
    while (!glfwWindowShouldClose(this->gameWindow)) { // Run the game loop until the game is ready to close.
        OnUpdate();

        // Begin: Rendering
        if (this->activeScene == NULL) // Is there anything to render?
            continue;

        // Clear color for window.
        Color clearColor = this->activeScene->GetBgColor().ToOpenGLFormat();
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // Iterate through each entity with a MeshRenderer
        std::vector<Entity*> meshEntities = this->activeScene->FindEntitiesWithComponent<MeshRenderer>();
        for (unsigned int i = 0; i < meshEntities.size(); ++i) {
            MeshRenderer * component = meshEntities[i]->FindComponent<MeshRenderer>();
            if (component != NULL) {
                component->Render();
            }
        }

        //  Swap buffers and poll IO
        glfwSwapBuffers(this->gameWindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return;
}
