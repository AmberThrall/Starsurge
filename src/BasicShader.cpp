#include "../include/Shader.h"

const char * source = R"(
    Shader {
        Uniforms {
            color fragColor = color(1,0,0,1);
            sampler2D Texture;
        }
        Pass {
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
                return texture(Texture, vs_out.UV) * vs_out.Color;
                //return fragColor;
                //return vs_out.Color;
            }
        }
    }
)";


Starsurge::Shader Starsurge::Shaders::BasicShader(source);
