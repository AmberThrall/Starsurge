#pragma once
#include <vector>
#include "Vector.h"
#include "Color.h"
//#include "Matrix.h"

namespace Starsurge {
    struct Vertex {
        Vector3 Position;
        Vector3 Normal;
        Vector2 UV;
        Color Color;
    };

    class Mesh {
    public:
        Mesh() {};
        Mesh(std::vector<Vertex> t_vertices, std::vector<unsigned int> t_indices);
        void RebuildMesh();

        unsigned int GetVAO();
        unsigned int GetVBO();
        unsigned int GetEBO();
        unsigned int NumberOfVertices();
        unsigned int NumberOfIndices();
        std::vector<Vertex> GetVertices();
        std::vector<unsigned int> GetIndices();
    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
    };
}
