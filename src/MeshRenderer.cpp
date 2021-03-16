#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Scene.h"
#include "../include/Entity.h"
#include "../include/Transform.h"
#include "../include/Camera.h"
#include "../include/MeshRenderer.h"
#include "../include/Light.h"

Starsurge::MeshRenderer::MeshRenderer(Mesh * t_mesh, Material * t_mat) : Component(typeid(MeshRenderer).name()) {
    this->mesh = t_mesh;
    this->material = t_mat;
}

void Starsurge::MeshRenderer::Render() {
    // Render each pass.
    for (unsigned int i = 0; i < this->material->GetShader()->NumberOfPasses(); ++i) {
        // Setup the built in uniforms.
        Transform * transform = GetOwner()->FindComponent<Transform>();
        if (transform) {
            this->material->GetUniform(i, "MATRIX_MODEL")->SetData(transform->AsMatrix());
            // TODO: Not calculate MATRIX_NORMAL every frame.
            if (transform->Scaling / transform->Scaling.x != Vector3(1,1,1)) {
                this->material->GetUniform(i, "MATRIX_NORMAL")->SetData(transform->AsMatrix().Inverse().Transpose().Resize<3,3>());
            }
            else {
                this->material->GetUniform(i, "MATRIX_NORMAL")->SetData(Matrix3::Identity());
            }
        }
        Entity * camera = Scene::Inst().GetActiveCamera();
        if (camera) {
            this->material->GetUniform(i, "MATRIX_VIEW")->SetData(camera->FindComponent<Camera>()->GetViewMatrix());
            this->material->GetUniform(i, "MATRIX_PROJ")->SetData(camera->FindComponent<Camera>()->GetProjMatrix());
            Transform * transform = camera->FindComponent<Transform>();
            if (transform) {
                this->material->GetUniform(i, "CAMERA")->SetData(transform->Position);
            }
        }

        // Now the lights.
        std::vector<Entity*> lights = Scene::Inst().FindEntitiesWithComponent<Light>();
        this->material->GetUniform(i, "NUM_LIGHTS")->SetData(lights.size());
        for (unsigned int j = 0; j < lights.size(); ++j) {
            lights[j]->FindComponent<Light>()->Apply(this->material, i, j);
        }

        // Apply the material (uniforms + shader program)
        this->material->Apply(i);

        // Actually render the mesh.
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
