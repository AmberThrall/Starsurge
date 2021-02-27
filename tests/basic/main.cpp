#include "../../include/Engine.h"
using namespace Starsurge;

class BasicGame : public Starsurge::Game {
public:
    BasicGame() : Game("Basic Game") { }
    ~BasicGame() { }
protected:
    void OnInitialize() {
        Scene * scene = new Scene();
        SetScene(scene);
        scene->SetBgColor(Color(51,76,76,255));

        Entity * triangle = new Entity("Triangle");
        triangle_mesh = Mesh::Triangle(Vector3(-0.5f, -0.5f, 0.0f), Vector3(0.5f, -0.5f, 0.0f), Vector3(0.0f, 0.5f, 0.0f));
        triangle_mat = Material(&Shaders::BasicShader);
        triangle->AddComponent<MeshRenderer>(new MeshRenderer(&triangle_mesh, &triangle_mat));

        scene->AddEntity(triangle);
    }

    void OnUpdate() {

    }
private:
    Mesh triangle_mesh;
    Material triangle_mat;
};

void main() {
    std::cout << Starsurge::GetVersion() << std::endl;

    BasicGame game;
    game.Run();
}
