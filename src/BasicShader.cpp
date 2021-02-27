#include "../include/Shader.h"

const char * source =
    "vertex {"
    "   vec4 vertex() {\n"
    "       return vec4(VERTEX.x, VERTEX.y, VERTEX.z, 1.0f);\n"
    "   }\n"
    "}\n"
    "fragment {"
    "   vec4 fragment() {\n"
    "       return vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "   }\n"
    "}\0";

Starsurge::Shader Starsurge::Shaders::BasicShader(source);
