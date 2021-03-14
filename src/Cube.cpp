#include "../include/Mesh.h"

Starsurge::Mesh * Starsurge::Mesh::Cube() {
    std::vector<Vector3> Positions = {
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3( 0.5f,  0.5f, -0.5f),
        Vector3( 0.5f, -0.5f, -0.5f),
        Vector3( 0.5f,  0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(-0.5f,  0.5f, -0.5f),

        Vector3(-0.5f, -0.5f,  0.5f),
        Vector3( 0.5f, -0.5f,  0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3(-0.5f,  0.5f,  0.5f),
        Vector3(-0.5f, -0.5f,  0.5f),

        Vector3(-0.5f,  0.5f,  0.5f),
        Vector3(-0.5f,  0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(-0.5f, -0.5f,  0.5f),
        Vector3(-0.5f,  0.5f,  0.5f),

        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3( 0.5f, -0.5f, -0.5f),
        Vector3( 0.5f,  0.5f, -0.5f),
        Vector3( 0.5f, -0.5f, -0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3( 0.5f, -0.5f,  0.5f),

        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3( 0.5f, -0.5f, -0.5f),
        Vector3( 0.5f, -0.5f,  0.5f),
        Vector3( 0.5f, -0.5f,  0.5f),
        Vector3(-0.5f, -0.5f,  0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),

        Vector3(-0.5f,  0.5f, -0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3( 0.5f,  0.5f, -0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3(-0.5f,  0.5f, -0.5f),
        Vector3(-0.5f,  0.5f,  0.5f),
    };
    std::vector<Vector2> UV = {
        Vector2(0.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 0.0f),
        Vector2(0.0f, 1.0f),

        Vector2(0.0f, 0.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 0.0f),

        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 0.0f),
        Vector2(1.0f, 0.0f),

        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 0.0f),

        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 0.0f),
        Vector2(0.0f, 1.0f),

        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 0.0f),
    };
    std::vector<Vector3> Normals = {
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),

        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),

        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),

        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),

        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),

        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
    };

    std::vector<Vertex> vertices;
    for (size_t i = 0; i < Positions.size(); ++i) {
        Vertex v;
        v.Position = Positions[i];
        v.Normal = Normals[i];
        v.UV = UV[i];
        v.Color = Colors::WHITE;
        vertices.push_back(v);
    }
    std::vector<unsigned int> indices;
    return new Mesh(vertices, indices);
}
