#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Input.h"
#include "../include/Logging.h"

void onKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    for (auto it = Starsurge::Input::Inst().keyCallbacks.begin(); it != Starsurge::Input::Inst().keyCallbacks.end(); it++) {
        int targetKey = std::get<0>(it->second.storage);
        if (targetKey == key || targetKey <= 0) {
            it->second.Call(key, action, mods);
        }
    }
}

void onMouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
    for (auto it = Starsurge::Input::Inst().mouseButtonCallbacks.begin(); it != Starsurge::Input::Inst().mouseButtonCallbacks.end(); it++) {
        int targetButton = std::get<0>(it->second.storage);
        if (targetButton == button || targetButton < 0) {
            it->second.Call(button, action, mods);
        }
    }
}

void onMousePosCallback(GLFWwindow * window, double x, double y) {
    for (auto it = Starsurge::Input::Inst().mousePosCallbacks.begin(); it != Starsurge::Input::Inst().mousePosCallbacks.end(); it++) {
        it->second.Call(x, y);
    }
}

void onScrollCallback(GLFWwindow * window, double x, double y) {
    for (auto it = Starsurge::Input::Inst().scrollCallbacks.begin(); it != Starsurge::Input::Inst().scrollCallbacks.end(); it++) {
        it->second.Call(x, y);
    }
}

Starsurge::Input& Starsurge::Input::Inst() {
    static Input instance;
    return instance;
}

unsigned int Starsurge::Input::KeyCallback(int key, std::function<void(int, int, int, void*)> function, void * t_data) {
    //Find the first available id.
    int id = 0;
    while (this->keyCallbacks.find(id) != this->keyCallbacks.end()) {
        id += 1;
    }
    Callback<TypeList<int>, TypeList<int, int, int>> callback;
    callback.Register(function, t_data);
    std::get<0>(callback.storage) = key;
    keyCallbacks[id] = callback;
    return id;
}

unsigned int Starsurge::Input::KeyCallback(std::function<void(int, int, int, void*)> function, void * t_data) {
    return KeyCallback(0, function, t_data);
}

unsigned int Starsurge::Input::MouseButtonCallback(int button, std::function<void(int, int, int, void*)> function, void * t_data) {
    //Find the first available id.
    int id = 0;
    while (this->mouseButtonCallbacks.find(id) != this->mouseButtonCallbacks.end()) {
        id += 1;
    }
    Callback<TypeList<int>, TypeList<int, int, int>> callback;
    callback.Register(function, t_data);
    std::get<0>(callback.storage) = button;
    mouseButtonCallbacks[id] = callback;
    return id;
}

unsigned int Starsurge::Input::MouseButtonCallback(std::function<void(int, int, int, void*)> function, void * t_data) {
    return MouseButtonCallback(-1, function, t_data);
}

unsigned int Starsurge::Input::MousePosCallback(std::function<void(double, double, void*)> function, void * t_data) {
    //Find the first available id.
    int id = 0;
    while (this->mousePosCallbacks.find(id) != this->mousePosCallbacks.end()) {
        id += 1;
    }
    Callback<TypeList<>, TypeList<double, double>> callback;
    callback.Register(function, t_data);
    mousePosCallbacks[id] = callback;
    return id;
}

unsigned int Starsurge::Input::ScrollCallback(std::function<void(double, double, void*)> function, void * t_data) {
    //Find the first available id.
    int id = 0;
    while (this->scrollCallbacks.find(id) != this->scrollCallbacks.end()) {
        id += 1;
    }
    Callback<TypeList<>, TypeList<double, double>> callback;
    callback.Register(function, t_data);
    scrollCallbacks[id] = callback;
    return id;
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

void Starsurge::Input::HideCursor() {
    glfwSetInputMode(this->gameWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Starsurge::Input::LockCursor() {
    glfwSetInputMode(this->gameWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Starsurge::Input::UnlockCursor() {
    glfwSetInputMode(this->gameWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Starsurge::Input::Setup(GLFWwindow * window) {
    this->gameWindow = window;
    glfwSetKeyCallback(this->gameWindow, onKeyCallback);
    glfwSetMouseButtonCallback(this->gameWindow, onMouseButtonCallback);
    glfwSetCursorPosCallback(this->gameWindow, onMousePosCallback);
    glfwSetScrollCallback(this->gameWindow, onScrollCallback);
}
