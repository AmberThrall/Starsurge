#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Engine.h"

Starsurge::Game::Game(std::string t_gamename) : gamename (t_gamename) {
}

Starsurge::Game::~Game() {

}

void Starsurge::Game::Run() {
    Starsurge::Log("Launching GLFW Window...");

    // Initialize OpenGL version 3.3
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create our window
    this->gameWindow = glfwCreateWindow(800, 600, this->gamename.c_str(), NULL, NULL);
    if (this->gameWindow == NULL) {
        Starsurge::FatalError("Failed to create game window.");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(this->gameWindow);

    // Initailize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Starsurge::FatalError("Failed to initialize GLAD.");
        return;
    }

    glViewport(0, 0, 800, 600);

    OnInitialize();
    GameLoop();
}

void Starsurge::Game::GameLoop() {
    while (!glfwWindowShouldClose(this->gameWindow)) { // Run the game loop until the game is ready to close.
        glfwSwapBuffers(this->gameWindow);
        glfwPollEvents();

        OnUpdate();
    }

    glfwTerminate();
    return;
}
