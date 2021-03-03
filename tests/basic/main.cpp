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

        Entity * square = new Entity("Triangle");
        square_mesh = Mesh::Quad(Vector3(0.5, 0.5, 0.0), Vector3(0.5, -0.5, 0), Vector3(-0.5, -0.5, 0.0), Vector3(-0.5, 0.5, 0.0));
        square_mat = Material(&Shaders::BasicShader);
        square->AddComponent<MeshRenderer>(new MeshRenderer(&square_mesh, &square_mat));

        scene->AddEntity(square);
    }

    void OnUpdate() {
        MaterialData * uniform_color = square_mat.GetUniform("color");
        uniform_color->SetData(Color(0,255.0f*(sin(glfwGetTime()) / 2.0 + 0.5f),0,255));
    }
private:
    Mesh square_mesh;
    Material square_mat;
};

void main() {
    std::cout << Starsurge::GetVersion() << std::endl;

    BasicGame game;
    game.Run();
}
