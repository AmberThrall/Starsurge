#include "../include/Color.h"

Starsurge::Color::Color() : Vector4(0,0,0,255) {

}

Starsurge::Color::Color(float r, float g, float b, float a) : Vector4(r,g,b,a) {
    // TODO: Clamp 0-255.
}

Starsurge::Color Starsurge::Color::ToOpenGLFormat() {
    Color ret(*this);
    ret[0] /= 255.0;
    ret[1] /= 255.0;
    ret[2] /= 255.0;
    ret[3] /= 255.0;
    return ret;
}
