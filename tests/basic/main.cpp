#include "../../include/Engine.h"
using namespace Starsurge;

class BasicGame : public Starsurge::Game {
public:
    BasicGame() : Game("Basic Game") { }
    ~BasicGame() { }
protected:
    void OnInitialize() {
        Vector3 v1 = {1,2,3};
        Vector3 v2 = {4,5,6};
        Vector3 v3 = v1 + v2;
        Vector3 v4 = v3 - v2;
        Vector<4> v5 = v1.Resize<4>();
        Vector3 v6 = v1.Unit();
        Vector3 v7 = Vector3::CrossProduct(v1, v2);
        Log(v1.ToString()+"+"+v2.ToString()+"="+v3.ToString());
        Log(v3.ToString()+"-"+v2.ToString()+"="+v4.ToString());
        Log(v1.ToString()+"=="+v1.ToString()+"? "+std::string((v1==v1) ? "True" : "False"));
        Log(v1.ToString()+"!="+v5.ToString()+"? "+std::string((v1!=v6) ? "True" : "False"));
        Log(v1.ToString()+" dot "+v6.ToString()+"="+std::to_string(Vector3::Dot(v1,v6)));
        Log("|"+v1.ToString()+"| = "+std::to_string(v1.Magnitude()));
        Log("|"+v6.ToString()+"| = "+std::to_string(v6.Magnitude()));
        Log(v1.ToString()+"x"+v2.ToString()+"="+v7.ToString());
    }

    void OnUpdate() {

    }
};

void main() {
    std::cout << Starsurge::GetVersion() << std::endl;

    BasicGame game;
    game.Run();
}
