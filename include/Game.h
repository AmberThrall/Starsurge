#pragma once
#include <GLFW/glfw3.h>

namespace Starsurge {
    class Game {
    public:
        Game(std::string t_gamename);
        ~Game();
        void Run();
    protected:
        virtual void OnInitialize() = 0;
        virtual void OnUpdate() = 0;

        std::string gamename;
    private:
        void GameLoop();
        GLFWwindow * gameWindow;
    };
}
