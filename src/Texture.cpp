#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/Texture.h"

Starsurge::Texture::Texture() {
    glGenTextures(1, &texture);
}
Starsurge::Texture::Texture(std::vector<Color> pixels, unsigned int width, unsigned int height) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    float * data = new float[pixels.size()*4];
    unsigned int index = 0;
    for (unsigned int i = 0; i < pixels.size(); ++i) {
        Color col = pixels[i].ToOpenGLFormat();
        data[index++] = col[0];
        data[index++] = col[1];
        data[index++] = col[2];
        data[index++] = col[3];
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    SetWrapS(GL_REPEAT);
    SetWrapT(GL_REPEAT);
    SetFilterMin(GL_LINEAR);
    SetFilterMag(GL_LINEAR);
    SetBorderColor(Colors::WHITE);
}

void Starsurge::Texture::SetPixels(std::vector<Color> pixels, unsigned int width, unsigned int height) {
    glBindTexture(GL_TEXTURE_2D, texture);

    float * data = new float[pixels.size()*4];
    unsigned int index = 0;
    for (unsigned int i = 0; i < pixels.size(); ++i) {
        Color col = pixels[i].ToOpenGLFormat();
        data[index++] = col[0];
        data[index++] = col[1];
        data[index++] = col[2];
        data[index++] = col[3];
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Starsurge::Texture::BindTexture(unsigned int textureSlot) {
    if (textureSlot == 0) { glActiveTexture(GL_TEXTURE0); }
    else if (textureSlot == 1) { glActiveTexture(GL_TEXTURE1); }
    else if (textureSlot == 2) { glActiveTexture(GL_TEXTURE2); }
    else if (textureSlot == 3) { glActiveTexture(GL_TEXTURE3); }
    else if (textureSlot == 4) { glActiveTexture(GL_TEXTURE4); }
    else if (textureSlot == 5) { glActiveTexture(GL_TEXTURE5); }
    else if (textureSlot == 6) { glActiveTexture(GL_TEXTURE6); }
    else if (textureSlot == 7) { glActiveTexture(GL_TEXTURE7); }
    else if (textureSlot == 8) { glActiveTexture(GL_TEXTURE8); }
    else if (textureSlot == 9) { glActiveTexture(GL_TEXTURE9); }
    else if (textureSlot == 10) { glActiveTexture(GL_TEXTURE10); }
    else if (textureSlot == 11) { glActiveTexture(GL_TEXTURE11); }
    else if (textureSlot == 12) { glActiveTexture(GL_TEXTURE12); }
    else if (textureSlot == 13) { glActiveTexture(GL_TEXTURE13); }
    else if (textureSlot == 14) { glActiveTexture(GL_TEXTURE14); }
    else if (textureSlot == 15) { glActiveTexture(GL_TEXTURE15); }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);

    Color col = borderColor.ToOpenGLFormat();
    float colArray[] = { col[0], col[1], col[2], col[3] };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colArray);
}

void Starsurge::Texture::SetWrapS(int option) {
    wrapS = option;
}
void Starsurge::Texture::SetWrapT(int option) {
    wrapT = option;
}
void Starsurge::Texture::SetFilterMin(int option) {
    filterMin = option;
}
void Starsurge::Texture::SetFilterMag(int option) {
    filterMag = option;
}
void Starsurge::Texture::SetBorderColor(Color color) {
    borderColor = color;
}
int Starsurge::Texture::GetWrapS() {
    return wrapS;
}
int Starsurge::Texture::GetWrapT() {
    return wrapT;
}
int Starsurge::Texture::GetFilterMin() {
    return filterMin;
}
int Starsurge::Texture::GetFilterMag() {
    return filterMag;
}
Starsurge::Color Starsurge::Texture::GetBorderColor() {
    return borderColor;
}
