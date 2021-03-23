#pragma once
#include <vector>
#include "Color.h"
#include "Framebuffer.h"

namespace Starsurge {
    // Predefine classes due to circular inclusion.
    class Framebuffer;

    class Texture {
    public:
        Texture();
        Texture(std::vector<Color> pixels, unsigned int width, unsigned int height);
        Texture(Framebuffer * framebuffer);

        void SetPixels(std::vector<Color> pixels, unsigned int width, unsigned int height);
        void SetPixels(Framebuffer * framebuffer);
        void BindTexture(unsigned int textureSlot);

        void SetWrapS(int option);
        void SetWrapT(int option);
        void SetFilterMin(int option);
        void SetFilterMag(int option);
        void SetBorderColor(Color color);
        int GetWrapS();
        int GetWrapT();
        int GetFilterMin();
        int GetFilterMag();
        Color GetBorderColor();

        unsigned int GetTexture();
    private:
        unsigned int texture;
        int wrapS;
        int wrapT;
        int filterMin;
        int filterMag;
        Color borderColor;
    };
}
