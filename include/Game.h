#pragma once
#include "imgui/imgui.h"
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "Rect.h"

namespace Starsurge {
    class Game {
    public:
        Game(std::string t_gamename);
        ~Game();
        void Run();
        void CloseGame();

        Framebuffer * GetFramebuffer();
        void AddPostProcessingEffect(Shader * shader);
    protected:
        virtual void OnInitialize() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnRender();

        std::string gamename;
    private:
        void GameLoop();
        GLFWwindow * gameWindow;
        Framebuffer * framebuffer;
        Framebuffer * framebuffer2;
        Shader basicPPE;
        unsigned int screenVAO, screenVBO;
        Rect viewport;
        std::vector<Shader*> postProcessingEffects;
    };
}
