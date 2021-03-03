#pragma once
#include <vector>
#include "Vector.h"

namespace Starsurge {
    struct Vertex {
        Vector3 Position;
        Vector3 Normal;
        Vector2 UV;
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

        static Mesh Triangle(Vector3 pt1, Vector3 pt2, Vector3 pt3);
        static Mesh Quad(Vector3 pt1, Vector3 pt2, Vector3 pt3, Vector3 pt4);
    private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
    };
}
