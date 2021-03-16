#include "../include/Shader.h"

Starsurge::Shader Starsurge::Shaders::Phong(R"(
    Shader {
        Pass {
            struct Material {
                sampler2D diffuse;
                sampler2D specular;
                float shininess;
            };
            uniform Material material;

            float Attenuation(float distance, float constant, float linear, float quadratic) {
                return (1.0 / (constant + linear*distance + quadratic*distance*distance));
            }

            color CalculatePointLight(Light light, vec3 normal, vec3 fragPos, float shininess, color mat_diffuse, color mat_specular) {
                vec3 lightDir = normalize(light.position - fragPos);
                vec3 viewDir = normalize(CAMERA - fragPos);
                // Diffuse:
                float diff = max(dot(normal, lightDir), 0.0);
                // Specular:
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                // Attenuation
                float distance = length(light.position - fragPos);
                float attenuation = Attenuation(distance, light.constant, light.linear, light.quadratic);

                vec3 ambient = vec3(light.ambient * mat_diffuse) * attenuation;
                vec3 diffuse = vec3(light.diffuse * diff * mat_diffuse) * attenuation;
                vec3 specular = vec3(light.specular * spec * mat_specular) * attenuation;
                return color(ambient + diffuse + specular, 1.0);
            }

            color CalculateDirLight(Light light, vec3 normal, vec3 fragPos, float shininess, color mat_diffuse, color mat_specular) {
                vec3 lightDir = normalize(-light.direction);
                vec3 viewDir = normalize(CAMERA - fragPos);
                // Diffuse:
                float diff = max(dot(normal, lightDir), 0.0);
                // Specular:
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

                vec3 ambient = vec3(light.ambient * mat_diffuse);
                vec3 diffuse = vec3(light.diffuse * diff * mat_diffuse);
                vec3 specular = vec3(light.specular * spec * mat_specular);
                return color(ambient + diffuse + specular, 1.0);
            }

            color CalculateSpotlight(Light light, vec3 normal, vec3 fragPos, float shininess, color mat_diffuse, color mat_specular) {
                vec3 lightDir = normalize(light.position-fragPos);
                vec3 viewDir = normalize(CAMERA - fragPos);
                float theta = dot(lightDir, normalize(-light.direction));
                float epsilon = light.innerRadius - light.outerRadius;
                float intensity = clamp((theta - light.outerRadius) / epsilon, 0.0, 1.0);

                // Diffuse:
                float diff = max(dot(normal, lightDir), 0.0);
                // Specular:
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

                vec3 ambient = vec3(light.ambient * mat_diffuse);
                vec3 diffuse = vec3(light.diffuse * diff * mat_diffuse) * intensity;
                vec3 specular = vec3(light.specular * spec * mat_specular) * intensity;
                return color(ambient + diffuse + specular, 1.0);
            }

            varying struct VS_OUT {
                vec3 Position;
                vec3 Normal;
                vec2 UV;
                color Color;
            } vs_out;

            vec4 vertex(VertexData v) {
                vs_out.Position = v.Position;
                vs_out.Normal = MATRIX_NORMAL*v.Normal;
                vs_out.UV = v.UV;
                vs_out.Color = v.Color;
                return MATRIX_PROJ*MATRIX_VIEW*MATRIX_MODEL*vec4(v.Position, 1.0f);
            }

            color fragment() {
                vec3 normal = normalize(vs_out.Normal);
                vec3 FragPos = vec3(MATRIX_MODEL * vec4(vs_out.Position, 1.0f));
                color diffuse = texture(material.diffuse, vs_out.UV);
                color specular = texture(material.specular, vs_out.UV);

                // Lighting
                color result = color(0,0,0,1);
                for (int i = 0; i < NUM_LIGHTS; ++i) {
                    if (LIGHTS[i].type == POINT_LIGHT) {
                        result += CalculatePointLight(LIGHTS[i], normal, FragPos, material.shininess, diffuse, specular);
                    }
                    else if (LIGHTS[i].type == DIRECTIONAL_LIGHT) {
                        result += CalculateDirLight(LIGHTS[i], normal, FragPos, material.shininess, diffuse, specular);
                    }
                    else if (LIGHTS[i].type == SPOTLIGHT) {
                        result += CalculateSpotlight(LIGHTS[i], normal, FragPos, material.shininess, diffuse, specular);
                    }
                }
                result.a = 1;

                return result;
            }
        }
    }
)");
