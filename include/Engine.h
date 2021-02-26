#pragma once
#include <iostream>
#include <string>

namespace Starsurge {
    std::string GetVersion();
    void Log(std::string message);
    void Error(std::string message);
    void FatalError(std::string message);
}

#include "Game.h"
#include "Scene.h"
#include "Vector.h"
