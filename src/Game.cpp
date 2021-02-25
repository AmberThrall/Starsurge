#include <GLFW/glfw3.h>
#include "../include/Engine.h"

Starsurge::Game::Game() {

}

Starsurge::Game::~Game() {

}

void Starsurge::Game::Run() {
    Starsurge::Log("Launching GLFW Window...");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->gameWindow = glfwCreateWindow(800, 600, "Starsurge", NULL, NULL);
    if (this->gameWindow == NULL) {
        Starsurge::FatalError("Failed to create game window.");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(this->gameWindow);

    OnInitialize();
    GameLoop();
}

void Starsurge::Game::GameLoop() {
    while (!glfwWindowShouldClose(this->gameWindow)) {
        glfwSwapBuffers(this->gameWindow);
        glfwPollEvents();

        OnUpdate();
    }

    glfwTerminate();
    return;
}
