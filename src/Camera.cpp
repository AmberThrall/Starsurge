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

void Starsurge::Camera::LookAt(Vector3 t_target) {
    this->target = t_target;
    Vector3 pos = Vector3(0,0,0);
    Vector3 up = Vector3::Up();
    Matrix4 eye = Matrix4::Translate(pos);
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        pos = transform->Position;
    }

    Vector3 dir = pos - this->target;
    dir.Normalize();
    Vector3 right = Vector3::CrossProduct(up, dir);
    right.Normalize();
    up = Vector3::CrossProduct(dir, right);
    this->viewMatrix = Matrix4::LookAt(right, up, dir, pos);
}

void Starsurge::Camera::SetOrthographic(float left, float right, float bottom, float top, float near, float far) {
    this->projMatrix = Matrix4::Orthographic(left, right, bottom, top, near, far);
}

void Starsurge::Camera::SetPerspective(float fov, float near, float far) {
    Vector2 windowSize = GameSettings::Inst().GetWindowSize();
    this->projMatrix = Matrix4::Perspective(Radians(fov), windowSize.x / windowSize.y, near, far);
}

Starsurge::Matrix4 Starsurge::Camera::GetViewMatrix() {
    return this->viewMatrix;
}

Starsurge::Matrix4 Starsurge::Camera::GetProjMatrix() {
    return this->projMatrix;
}

Starsurge::Vector3 Starsurge::Camera::Forwards() {
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        Vector3 v = this->target - transform->Position;
        v.Normalize();
        return v;
    }
    return Vector3(0,0,0);
}

Starsurge::Vector3 Starsurge::Camera::Up() {
    return Vector3(0,1,0);
}

void Starsurge::Camera::OnUpdate() {
    Transform * transform = GetOwner()->FindComponent<Transform>();
    if (transform) {
        if (transform->HasMoved()) {
            LookAt(this->target);
        }
    }
}
