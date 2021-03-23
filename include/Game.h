#pragma once
#include "imgui/imgui.h"
#include <GLFW/glfw3.h>
#include "Scene.h"

namespace Starsurge {
    class Game {
    public:
        Game(std::string t_gamename);
        ~Game();
        void Run();
        void CloseGame();
    protected:
        virtual void OnInitialize() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnRender();

        std::string gamename;
    private:
        void GameLoop();
        GLFWwindow * gameWindow;
    };
}
