#include <GLFW/glfw3.h>

namespace Starsurge {
    class Game {
    public:
        Game();
        ~Game();
        void Run();
    protected:
        virtual void OnInitialize() = 0;
        virtual void OnUpdate() = 0;
    private:
        void GameLoop();

        GLFWwindow * gameWindow;
    };
}
