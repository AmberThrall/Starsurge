#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Mesh.h"

Starsurge::Mesh::Mesh(std::vector<Vertex> t_vertices, std::vector<unsigned int> t_indices) : vertices(t_vertices), indices(t_indices) {
    RebuildMesh();
}

void Starsurge::Mesh::RebuildMesh() {
    // float * gl_vertices = new float[NumberOfVertices()*3];
    //
    // for (unsigned int i = 0; i < NumberOfVertices(); ++i) {
    //     gl_vertices[3*i] = this->vertices[i].Position[0];
    //     gl_vertices[3*i+1] = this->vertices[i].Position[1];
    //     gl_vertices[3*i+2] = this->vertices[i].Position[2];
    // }

    float gl_vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertices), gl_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

unsigned int Starsurge::Mesh::GetVAO() {
    return this->VAO;
}

unsigned int Starsurge::Mesh::GetVBO() {
    return this->VBO;
}

unsigned int Starsurge::Mesh::NumberOfVertices() {
    return this->vertices.size();
}

Starsurge::Mesh Starsurge::Mesh::Triangle(Vector3 pt1, Vector3 pt2, Vector3 pt3) {
    Vertex v1;
    v1.Position = pt1;
    v1.Normal = Vector3(0,0,0);
    v1.UV = Vector2(0,0);
    Vertex v2;
    v2.Position = pt2;
    v2.Normal = Vector3(0,0,0);
    v2.UV = Vector2(0,0);
    Vertex v3;
    v3.Position = pt3;
    v3.Normal = Vector3(0,0,0);
    v3.UV = Vector2(0,0);
    std::vector<Vertex> vertices = { v1, v2, v3 };
    std::vector<unsigned int> indices;
    return Mesh(vertices, indices);
}
