#pragma once
#include <GLFW/glfw3.h>
#include "Vector.h"

namespace Starsurge {
    class GameSettings {
    public:
        static GameSettings& Inst();

        Vector2 GetWindowSize();
        void ResizeWindow(unsigned int w, unsigned int h);
    private:
        GameSettings() {};

        GLFWwindow * gameWindow;

        friend class Game;
    };
}
