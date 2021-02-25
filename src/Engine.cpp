#include "../include/Engine.h"

std::string Starsurge::GetVersion() {
    return "0.1";
}

void Starsurge::Log(std::string message) {
    std::cout << message << std::endl;
}

void Starsurge::FatalError(std::string message) {
    std::cout << "[FATAL] " <<  message << std::endl;
}
