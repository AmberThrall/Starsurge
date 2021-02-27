#include <string>
#include <iostream>

namespace Starsurge {
    std::string GetVersion();
    void Log(std::string message);
    void Error(std::string message);
    void FatalError(std::string message);
    void ShaderError(std::string message);
}
