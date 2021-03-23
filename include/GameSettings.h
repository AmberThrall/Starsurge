#pragma once
#include <GLFW/glfw3.h>
#include "Vector.h"
#include "Rect.h"

namespace Starsurge {
    class GameSettings {
    public:
        static GameSettings& Inst();

        Rect GetViewport();
        void SetViewport(Rect view);
        void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
        Vector2 GetWindowSize();
        void ResizeWindow(unsigned int w, unsigned int h);
    private:
        GameSettings() {};

        GLFWwindow * gameWindow;
        Rect viewport;

        friend class Game;
    };
}
