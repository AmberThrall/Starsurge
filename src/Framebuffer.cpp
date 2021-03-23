#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Framebuffer.h"
#include "../include/Rect.h"
#include "../include/GameSettings.h"

Starsurge::Framebuffer::Framebuffer(unsigned int width, unsigned int height) {
    this->size = Vector2(width, height);

    // Generate the framebuffer.
    glGenFramebuffers(1, &this->fbo);
    Bind();

    // Create the texture storage.
    this->texture = new Texture(this);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture->GetTexture(), 0);

    // Create a renderbuffer object.
    glGenRenderbuffers(1, &this->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    Unbind();
}
Starsurge::Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &this->fbo);
}

void Starsurge::Framebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}

void Starsurge::Framebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Starsurge::Framebuffer::IsComplete() {
    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void Starsurge::Framebuffer::Resize(unsigned int width, unsigned int height) {
    this->size = Vector2(width, height);
    Bind();

    // Set textures data
    this->texture->SetPixels(this);

    // Modify the renderbuffer object's storage.
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // Unbind everything and finish.
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    Unbind();
}

Starsurge::Vector2 Starsurge::Framebuffer::GetSize() {
    return this->size;
}
unsigned int Starsurge::Framebuffer::GetFramebuffer() {
    return this->fbo;
}
Starsurge::Texture * Starsurge::Framebuffer::GetTexture() {
    return this->texture;
}
unsigned int Starsurge::Framebuffer::GetRenderbuffer() {
    return this->rbo;
}
