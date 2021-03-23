#pragma once
#include "Component.h"
#include "Vector.h"
#include "Matrix.h"

namespace Starsurge {
    class Camera : public Component {
    public:
        Camera(float fov, float near, float far);
        Camera(float left, float right, float bottom, float top, float near, float far);

        void LookAt(Vector3 target);
        void SetOrthographic(float left, float right, float bottom, float top, float near, float far);
        void SetPerspective(float fov, float near, float far);
        void SetPerspective(float fov, float aspect, float near, float far);

        Matrix4 GetViewMatrix();
        Matrix4 GetProjMatrix();

        Vector3 Forward();
        Vector3 Up();
        Vector3 Right();
    protected:
        void OnUpdate();
    private:
        Matrix4 viewMatrix;
        Matrix4 projMatrix;
    };
}
