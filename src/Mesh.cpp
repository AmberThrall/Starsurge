#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Mesh.h"

Starsurge::Mesh::Mesh(std::vector<Vertex> t_vertices, std::vector<unsigned int> t_indices) : vertices(t_vertices), indices(t_indices) {
    RebuildMesh();
}

void Starsurge::Mesh::RebuildMesh() {
    float * gl_vertices = new float[NumberOfVertices()*12];

    unsigned int index = 0;
    for (unsigned int i = 0; i < NumberOfVertices(); ++i) {
        Color col = this->vertices[i].Color.ToOpenGLFormat();
        gl_vertices[index++] = this->vertices[i].Position[0];
        gl_vertices[index++] = this->vertices[i].Position[1];
        gl_vertices[index++] = this->vertices[i].Position[2];
        gl_vertices[index++] = this->vertices[i].Normal[0];
        gl_vertices[index++] = this->vertices[i].Normal[1];
        gl_vertices[index++] = this->vertices[i].Normal[2];
        gl_vertices[index++] = this->vertices[i].UV[0];
        gl_vertices[index++] = this->vertices[i].UV[1];
        gl_vertices[index++] = col[0];
        gl_vertices[index++] = col[1];
        gl_vertices[index++] = col[2];
        gl_vertices[index++] = col[3];
    }

    unsigned int * gl_indices;
    if (this->indices.size() > 0) {
        gl_indices = &this->indices[0];
    }

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    if (this->indices.size() > 0) {
        glGenBuffers(1, &this->EBO);
    }
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, NumberOfVertices()*12*sizeof(float), gl_vertices, GL_STATIC_DRAW);
    if (this->indices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumberOfIndices()*sizeof(unsigned int), gl_indices, GL_STATIC_DRAW);
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6*sizeof(float)));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

unsigned int Starsurge::Mesh::GetVAO() {
    return this->VAO;
}

unsigned int Starsurge::Mesh::GetVBO() {
    return this->VBO;
}

unsigned int Starsurge::Mesh::GetEBO() {
    return this->EBO;
}

unsigned int Starsurge::Mesh::NumberOfVertices() {
    return this->vertices.size();
}

unsigned int Starsurge::Mesh::NumberOfIndices() {
    return this->indices.size();
}

std::vector<Starsurge::Vertex> Starsurge::Mesh::GetVertices() {
    return this->vertices;
}
std::vector<unsigned int> Starsurge::Mesh::GetIndices() {
    return this->indices;
}
