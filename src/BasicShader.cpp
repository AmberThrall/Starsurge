#include "../include/Shader.h"

const char * source = R"(
    Shader {
        Pass {
            uniform color Color;
            uniform sampler2D Texture;

            varying struct VS_OUT {
                vec3 Position;
                vec3 Normal;
                vec2 UV;
                color Color;
            } vs_out;

            vec4 vertex(VertexData v) {
                vs_out.Position = v.Position;
                vs_out.Normal = v.Normal;
                vs_out.UV = v.UV;
                vs_out.Color = v.Color;
                return MATRIX_PROJ*MATRIX_VIEW*MATRIX_MODEL*vec4(v.Position, 1.0f);
            }

            color fragment() {
                return texture(Texture, vs_out.UV) * vs_out.Color * Color;
            }
        }
    }
)";


Starsurge::Shader Starsurge::Shaders::BasicShader(source);
