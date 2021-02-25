#include "../../include/Engine.h"

class BasicGame : public Starsurge::Game {
public:
    BasicGame() { }
    ~BasicGame() { }
protected:
    void OnInitialize() {

    }

    void OnUpdate() {

    }
};

void main() {
    std::cout << Starsurge::GetVersion() << std::endl;

    BasicGame game;
    game.Run();
}
