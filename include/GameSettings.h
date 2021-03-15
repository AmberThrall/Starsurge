#pragma once

namespace Starsurge {
    class GameSettings {
    public:
        static GameSettings& Inst();

        unsigned int GetWindowWidth();
        unsigned int GetWindowHeight();
        void ResizeWindow(unsigned int w, unsigned int h);
    private:
        GameSettings() {};
        unsigned int windowWidth, windowHeight;
    };
}
