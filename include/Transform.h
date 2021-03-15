#pragma once
#include "Component.h"
#include "Vector.h"
#include "Matrix.h"

namespace Starsurge {
    class Transform : public Component {
    public:
        Transform();
        Transform(Vector3 t_pos, Vector3 t_rot, Vector3 t_scale);
        Matrix4 AsMatrix();
        bool HasMoved();

        Vector3 Position;
        Vector3 Rotation;
        Vector3 Scaling;
    private:
        void UpdateTransform();

        Vector3 oldPosition;
        Vector3 oldRotation;
        Vector3 oldScaling;
        Matrix4 transformMatrix;
    };
}
