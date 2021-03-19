#include "../include/Transform.h"

Starsurge::Rotation3D::Rotation3D() {
    SetEulerAngles(Vector3(0,0,0));
}
Starsurge::Rotation3D::Rotation3D(EulerAngles euler) {
    SetEulerAngles(euler);
}
Starsurge::Rotation3D::Rotation3D(Quaternion quaternion) {
    SetQuaternion(quaternion);
}

Starsurge::EulerAngles Starsurge::Rotation3D::GetEulerAngles() {
    return this->eulerAngles;
}

Starsurge::Quaternion Starsurge::Rotation3D::GetQuaternion() {
    return this->quaternion;
}

void Starsurge::Rotation3D::SetEulerAngles(EulerAngles euler) {
    this->eulerAngles = euler;
    this->quaternion = Quaternion::FromEulerAngles(euler);
}

void Starsurge::Rotation3D::SetQuaternion(Quaternion q) {
    this->quaternion = q;
    this->eulerAngles = EulerAngles::FromQuaternion(q);
}

Starsurge::Transform::Transform() : Component(typeid(Transform).name()) {
    this->Position = Vector3(0,0,0);
    this->Rotation.SetEulerAngles(EulerAngles(0,0,0, XYZ));
    this->Scaling = Vector3(1,1,1);
    UpdateTransform();
}

Starsurge::Transform::Transform(Vector3 t_pos, EulerAngles t_rot, Vector3 t_scale) : Component(typeid(Transform).name()) {
    this->Position = t_pos;
    this->Rotation.SetEulerAngles(t_rot);
    this->Scaling = t_scale;
    UpdateTransform();
}

Starsurge::Matrix4 Starsurge::Transform::AsMatrix() {
    if (HasMoved()) {
        UpdateTransform();
    }
    return this->transformMatrix;
}

bool Starsurge::Transform::HasMoved() {
    return (this->Position != this->oldPosition || this->Rotation.GetQuaternion() != oldQuaternion || this->Scaling != this->oldScaling);
}

void Starsurge::Transform::UpdateTransform() {
    this->transformMatrix = Matrix4::Rotate(this->Rotation.GetQuaternion());
    this->transformMatrix *= Matrix4::Scale(this->Scaling);
    this->transformMatrix *= Matrix4::Translate(this->Position);

    // Keep track of the data so we don't matrix multiply every frame.
    this->oldPosition = this->Position;
    this->oldQuaternion = this->Rotation.GetQuaternion();
    this->oldScaling = this->Scaling;
}
