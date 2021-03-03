#include "../include/Shader.h"

const char * source =
    "uniform vec4 color;\n"
    "\n"
    "vec4 vertex(VertexData v) {\n"
    "   return vec4(v.Position.x, v.Position.y, v.Position.z, 1.0f);\n"
    "}\n"
    "\n"
    "vec4 fragment() {\n"
    "   return vertexColor;\n"
    "}\0";

Starsurge::Shader Starsurge::Shaders::BasicShader(source);
