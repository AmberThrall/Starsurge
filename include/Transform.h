#pragma once
#include "Component.h"
#include "Vector.h"
#include "Quaternion.h"
#include "EulerAngles.h"
#include "Matrix.h"

namespace Starsurge {
    class Rotation3D {
    public:
        Rotation3D();
        Rotation3D(EulerAngles eulerAngles);
        Rotation3D(Quaternion quaternion);
        EulerAngles GetEulerAngles();
        Quaternion GetQuaternion();
        void SetEulerAngles(EulerAngles angles);
        void SetQuaternion(Quaternion q);
    private:
        EulerAngles eulerAngles;
        Quaternion quaternion;
    };

    class Transform : public Component {
    public:
        Transform();
        Transform(Vector3 t_pos, EulerAngles t_rot, Vector3 t_scale);
        Matrix4 AsMatrix();
        bool HasMoved();

        Vector3 Position;
        Rotation3D Rotation;
        Vector3 Scaling;
    private:
        void UpdateTransform();

        Vector3 oldPosition;
        Quaternion oldQuaternion;
        Vector3 oldScaling;
        Matrix4 transformMatrix;
    };
}
