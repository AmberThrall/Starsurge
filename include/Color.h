#pragma once
#include "Vector.h"

namespace Starsurge {
    class Color : public Vector4 {
    public:
        Color();
        Color(float r, float g, float b, float a);

        Color ToOpenGLFormat();
    };
}
