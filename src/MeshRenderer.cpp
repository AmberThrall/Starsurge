#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/MeshRenderer.h"

Starsurge::MeshRenderer::MeshRenderer(Mesh * t_mesh, Material * t_mat) : Component(typeid(MeshRenderer).name()) {
    this->mesh = t_mesh;
    this->material = t_mat;
}

void Starsurge::MeshRenderer::Render() {
    for (unsigned int i = 0; i < this->material->GetShader()->NumberOfPasses(); ++i) {
        this->material->Apply(i);
        glBindVertexArray(this->mesh->GetVAO());
        //glDrawArrays(GL_TRIANGLES, 0, this->mesh->NumberOfVertices());
        glDrawElements(GL_TRIANGLES, this->mesh->NumberOfIndices(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
