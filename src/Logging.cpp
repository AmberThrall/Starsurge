#include "../include/Logging.h"

std::string Starsurge::GetVersion() {
    return "0.1";
}

void Starsurge::Log(std::string message) {
    std::cout << message << std::endl;
}

void Starsurge::Error(std::string message) {
    std::cout << "[Error] " << message << std::endl;
}

void Starsurge::FatalError(std::string message) {
    std::cout << "[FATAL] " <<  message << std::endl;
}

void Starsurge::ShaderError(std::string message) {
    std::cout << "[SHADER] " <<  message << std::endl;
}
