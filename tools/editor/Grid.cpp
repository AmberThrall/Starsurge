#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Grid.h"

Grid::Grid(float t_size) {
    CompileShader();
    SetSize(t_size);
}

void Grid::SetSize(float t_size) {
    this->size = t_size;
    BuildMesh(t_size);
}

void Grid::Render() {
    glUseProgram(this->shader);
    Entity * camera = Scene::Inst().GetActiveCamera();
    if (camera) {
        glUniformMatrix4fv(glGetUniformLocation(this->shader, "MATRIX_VIEW"), 1, GL_TRUE, camera->FindComponent<Camera>()->GetViewMatrix().Ptr());
        glUniformMatrix4fv(glGetUniformLocation(this->shader, "MATRIX_PROJ"), 1, GL_TRUE, camera->FindComponent<Camera>()->GetProjMatrix().Ptr());
    }
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "MATRIX_MODEL"), 1, GL_TRUE, Matrix4::Identity().Ptr());

    glBindVertexArray(this->VAO);
    glUniform4f(glGetUniformLocation(this->shader, "axisColor"), 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArraysInstanced(GL_LINES, 0, 2, this->numOffsets);
    glUniform4f(glGetUniformLocation(this->shader, "axisColor"), 0.0f, 1.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(this->shader, "MATRIX_MODEL"), 1, GL_TRUE, Matrix4::Rotate(Radians(90), Vector3(0,1,0)).Ptr());
    glDrawArraysInstanced(GL_LINES, 0, 2, this->numOffsets);
    glBindVertexArray(0);
}

void Grid::BuildMesh(float t_size) {
    const float bounds = 128;
    float vertices[] = {
        -bounds, 0,
        bounds, 0
    };
    std::vector<float> offsets;
    offsets.push_back(0);
    for (float y = t_size; y < bounds; y += t_size) {
        offsets.push_back(y);
        offsets.push_back(-y);
    }
    this->numOffsets = offsets.size();

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->IVBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, this->IVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size()*sizeof(float), &offsets[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1*sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
}

void Grid::CompileShader() {
    const char * vertex_src = R"(
        #version 330
        layout (location = 0) in vec2 aPosition;
        layout (location = 1) in float aOffset;
        uniform mat4 MATRIX_PROJ, MATRIX_VIEW, MATRIX_MODEL;
        out float offset;
        void main() {
            offset = aOffset;
            gl_Position = MATRIX_PROJ*MATRIX_VIEW*MATRIX_MODEL*vec4(aPosition.x, 0, aPosition.y + aOffset, 1.0);
        }
    )";
    const char * frag_src = R"(
        #version 330
        in float offset;
        out vec4 FragColor;
        uniform vec4 axisColor;
        void main() {
            if (offset == 0) {
                FragColor = axisColor;
            }
            else {
                FragColor = vec4(0.298, 0.298, 0.298, 1.0);
            }
        }
    )";

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_src, NULL);
    glCompileShader(vertexShader);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &frag_src, NULL);
    glCompileShader(fragShader);
    shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragShader);
    glLinkProgram(shader);
}
