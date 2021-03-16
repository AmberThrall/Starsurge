#pragma once
#include "Component.h"
#include "Vector.h"
#include "Color.h"
#include "Material.h"

namespace Starsurge {
    enum LightType {
        POINT_LIGHT = 0,
        DIRECTIONAL_LIGHT = 1,
        SPOTLIGHT = 2
    };

    class Light : public Component {
    private:
        struct PointData {
            float Constant;
            float Linear;
            float Quadratic;
        };

        struct DirectionalData {
            Vector3 Direction;
        };

        struct SpotlightData {
            Vector3 Direction;

            float InnerCone;
            float OuterCone;
        };
    public:
        Light(LightType t_type = POINT_LIGHT);

        LightType Type;

        Color Ambient;
        Color Diffuse;
        Color Specular;

        PointData PointLight;
        DirectionalData DirectionalLight;
        SpotlightData Spotlight;

        void Apply(Material * mat, unsigned int passno, unsigned int id);
    };
}
