#include "../include/GameSettings.h"
#include "../include/Entity.h"
#include "../include/Transform.h"
#include "../include/Camera.h"
#include "../include/Utils.h"

Starsurge::Camera::Camera(float fov, float near, float far) : Component(typeid(Camera).name()) {
    this->viewMatrix = Matrix4::Identity();
    SetPerspective(fov, near, far);
}

Starsurge::Camera::Camera(float left, float right, float bottom, float top, float near, float far) : Component(typeid(Camera).name()) {
    this->viewMatrix = Matrix4::Identity();
    SetOrthographic(left, right, bottom, top, near, far);
}

void Starsurge::Camera::LookAt(Vector3 target) {
    //https://stackoverflow.com/questions/52413464/look-at-quaternion-using-up-vector
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        Quaternion q;
        Vector3 eye = transform->Position;
        Vector3 forward = Vector3::Normalize(eye - target);
        Vector3 right = Vector3::Normalize(Vector3::CrossProduct(Vector3::Up(), forward));
        Vector3 up = Vector3::CrossProduct(forward, right);

        float trace = right.x + up.y + forward.z;
        if (trace > 0) {
            float s = 0.5 / sqrt(trace + 1.0);
            q = Quaternion(0.25 / s, (up.z - forward.y) * s, (forward.x - right.z) * s, (right.y - up.x) * s);
        }
        else if (right.x > up.y) {
            float s = 2 * sqrt(trace + right.x - up.y - forward.z);
            q = Quaternion((up.z - forward.y) / s, 0.25*s, (up.x + right.y) / s, (forward.x + right.z) / s);
        }
        else if (up.y > forward.z) {
            float s = 2 * sqrt(trace + up.y - right.x - forward.z);
            q = Quaternion((forward.x - right.z) / s, (up.x + right.y) / s, 0.25 * s, (forward.y + up.z) / s);
        }
        else {
            float s = 2 * sqrt(trace+forward.z - right.x - up.y);
            q = Quaternion((right.y - up.x) / s, (forward.x + right.z) / s, (forward.y + up.z) / s, 0.25 * s);
        }

        transform->Rotation.SetQuaternion(q);
    }
    this->viewMatrix = Matrix4::LookAt(Right(), Up(), Forward(), transform->Position);
}

void Starsurge::Camera::SetOrthographic(float left, float right, float bottom, float top, float near, float far) {
    this->projMatrix = Matrix4::Orthographic(left, right, bottom, top, near, far);
}

void Starsurge::Camera::SetPerspective(float fov, float near, float far) {
    Rect viewport = GameSettings::Inst().GetViewport();
    float width = viewport.GetMaximum().x-viewport.GetMinimum().x;
    float height = viewport.GetMaximum().y-viewport.GetMinimum().y;
    this->projMatrix = Matrix4::Perspective(Radians(fov), width / height, near, far);
}

void Starsurge::Camera::SetPerspective(float fov, float aspect, float near, float far) {
    this->projMatrix = Matrix4::Perspective(Radians(fov), aspect, near, far);
}

Starsurge::Matrix4 Starsurge::Camera::GetViewMatrix() {
    return this->viewMatrix;
}

Starsurge::Matrix4 Starsurge::Camera::GetProjMatrix() {
    return this->projMatrix;
}

Starsurge::Vector3 Starsurge::Camera::Forward() {
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        Quaternion q = transform->Rotation.GetQuaternion();
        Vector3 v = q.Rotate(Vector3::Forward());
        v.Normalize();
        return v;
    }
    return Vector3(0,0,0);
}

Starsurge::Vector3 Starsurge::Camera::Up() {
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        Quaternion q = transform->Rotation.GetQuaternion();
        Vector3 v = q.Rotate(Vector3::Up());
        v.Normalize();
        return v;
    }
    return Vector3(0,0,0);
}

Starsurge::Vector3 Starsurge::Camera::Right() {
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        Quaternion q = transform->Rotation.GetQuaternion();
        Vector3 v = q.Rotate(Vector3::Right());
        v.Normalize();
        return v;
    }
    return Vector3(0,0,0);
}

void Starsurge::Camera::OnUpdate() {
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        if (transform->HasMoved()) {
            // Remake the VIEW matrix.
            //LookAt(transform->Position + Forward());
            this->viewMatrix = Matrix4::LookAt(Right(), Up(), Forward(), transform->Position);
        }
    }
}
