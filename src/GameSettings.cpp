#include "../include/GameSettings.h"

Starsurge::GameSettings& Starsurge::GameSettings::Inst() {
    static GameSettings instance;
    return instance;
}

unsigned int Starsurge::GameSettings::GetWindowWidth() {
    return this->windowWidth;
}

unsigned int Starsurge::GameSettings::GetWindowHeight() {
    return this->windowHeight;
}

void Starsurge::GameSettings::ResizeWindow(unsigned int w, unsigned int h) {
    this->windowWidth = w;
    this->windowHeight = h;
}
