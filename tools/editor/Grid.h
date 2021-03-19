#include "../../include/Engine.h"
using namespace Starsurge;

class Grid {
public:
    Grid(float t_size = 0);

    void SetSize(float t_size);

    void Render();
private:
    float size;
    unsigned int VBO, IVBO, VAO;

    void BuildMesh(float t_size);
    void CompileShader();
    unsigned int numOffsets;
    unsigned int vertexShader, geomShader, fragShader, shader;
};
