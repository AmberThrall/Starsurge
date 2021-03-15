#pragma once
#include "Component.h"
#include "Vector.h"
#include "Matrix.h"

namespace Starsurge {
    class Camera : public Component {
    public:
        Camera(float fov, float near, float far);
        Camera(float left, float right, float bottom, float top, float near, float far);

        void Refresh();
        void LookAt(Vector3 t_target);
        void SetOrthographic(float left, float right, float bottom, float top, float near, float far);
        void SetPerspective(float fov, float near, float far);

        Matrix4 GetViewMatrix();
        Matrix4 GetProjMatrix();
    protected:
        void OnUpdate();
    private:
        Vector3 target;
        Matrix4 viewMatrix;
        Matrix4 projMatrix;
    };
}
