#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/GameSettings.h"

Starsurge::GameSettings& Starsurge::GameSettings::Inst() {
    static GameSettings instance;
    return instance;
}

Starsurge::Rect Starsurge::GameSettings::GetViewport() {
    return this->viewport;
}
void Starsurge::GameSettings::SetViewport(Rect view) {
    this->viewport = view;
    Vector2 min = Vector2::Round(view.GetMinimum());
    Vector2 max = Vector2::Round(view.GetMaximum());
    glViewport(min.x, min.y, max.x-min.x, max.y-min.y);
}
void Starsurge::GameSettings::SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
    SetViewport(Rect(x,y,x+w,y+h));
}

Starsurge::Vector2 Starsurge::GameSettings::GetWindowSize() {
    int w, h;
    glfwGetWindowSize(this->gameWindow, &w, &h);
    return Vector2(w, h);
}

void Starsurge::GameSettings::ResizeWindow(unsigned int w, unsigned int h) {
    glfwSetWindowSize(this->gameWindow, w, h);
    SetViewport(0, 0, w, h);
}
