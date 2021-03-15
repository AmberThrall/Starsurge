#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Input.h"

Starsurge::Input& Starsurge::Input::Inst() {
    static Input instance;
    return instance;
}

Starsurge::InputState Starsurge::Input::Key(InputKey key) {
    return (InputState)glfwGetKey(this->gameWindow, key);
}

Starsurge::InputState Starsurge::Input::MouseButton(InputMouseButton button) {
    return (InputState)glfwGetMouseButton(this->gameWindow, button);
}

Starsurge::Vector2 Starsurge::Input::GetCurserPos() {
    double x, y;
    glfwGetCursorPos(this->gameWindow, &x, &y);
    return Vector2(x,y);
}

void Starsurge::Input::MoveCursor(double x, double y) {
    glfwSetCursorPos(this->gameWindow, x, y);
}
