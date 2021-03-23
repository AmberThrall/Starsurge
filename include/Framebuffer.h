#pragma once
#include "Texture.h"

namespace Starsurge {
    // Predefine classes due to circular inclusion.
    class Texture;

    class Framebuffer {
    public:
        Framebuffer(unsigned int width, unsigned int height);
        ~Framebuffer();

        void Bind();
        void Unbind();
        bool IsComplete();
        void Resize(unsigned int width, unsigned int height);

        Vector2 GetSize();
        unsigned int GetFramebuffer();
        Texture * GetTexture();
        unsigned int GetRenderbuffer();
    private:
        Vector2 size;
        unsigned int fbo, rbo;
        Texture * texture;
    };
}
