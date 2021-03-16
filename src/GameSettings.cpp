#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/GameSettings.h"

Starsurge::GameSettings& Starsurge::GameSettings::Inst() {
    static GameSettings instance;
    return instance;
}

Starsurge::Vector2 Starsurge::GameSettings::GetWindowSize() {
    int w, h;
    glfwGetWindowSize(this->gameWindow, &w, &h);
    return Vector2(w, h);
}

void Starsurge::GameSettings::ResizeWindow(unsigned int w, unsigned int h) {
    glfwSetWindowSize(this->gameWindow, w, h);
}
