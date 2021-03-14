#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Entity.h"
#include "../include/Transform.h"
#include "../include/MeshRenderer.h"

Starsurge::MeshRenderer::MeshRenderer(Mesh * t_mesh, Material * t_mat) : Component(typeid(MeshRenderer).name()) {
    this->mesh = t_mesh;
    this->material = t_mat;
}

void Starsurge::MeshRenderer::Render() {
    // Setup the matrices.
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        this->material->GetUniform("MATRIX_MODEL")->SetData(transform->AsMatrix());
    }
    else {
        this->material->GetUniform("MATRIX_MODEL")->SetData(Matrix4::Identity());
    }

    // Render each pass.
    for (unsigned int i = 0; i < this->material->GetShader()->NumberOfPasses(); ++i) {
        this->material->Apply(i);
        glBindVertexArray(this->mesh->GetVAO());
        if (this->mesh->NumberOfIndices() > 0) {
            glDrawElements(GL_TRIANGLES, this->mesh->NumberOfIndices(), GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, this->mesh->NumberOfVertices());
        }
        glBindVertexArray(0);
    }
}
