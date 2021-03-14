#pragma once
#include "Vector.h"

namespace Starsurge {
    class Color : public Vector<4> {
    public:
        Color();
        Color(float r, float g, float b, float a);

        Color ToOpenGLFormat();
    };

    namespace Colors {
        const Color WHITE = Color(255, 255, 255, 255);
    }
}
