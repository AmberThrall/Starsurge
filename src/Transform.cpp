#include "../include/Transform.h"

Starsurge::Transform::Transform() : Component(typeid(Transform).name()) {
    this->Position = Vector3(0,0,0);
    this->Rotation = Vector3(0,0,0);
    this->Scaling = Vector3(1,1,1);
    UpdateTransform();
}

Starsurge::Transform::Transform(Vector3 t_pos, Vector3 t_rot, Vector3 t_scale) : Component(typeid(Transform).name()) {
    this->Position = t_pos;
    this->Rotation = t_rot;
    this->Scaling = t_scale;
    UpdateTransform();
}

Starsurge::Matrix4 Starsurge::Transform::AsMatrix() {
    if (this->Position != this->oldPosition || this->Rotation != this->oldRotation || this->Scaling != this->oldScaling) {
        UpdateTransform();
    }
    return this->transformMatrix;
}

void Starsurge::Transform::UpdateTransform() {
    this->transformMatrix = Matrix4::Scale(this->Scaling)*Matrix4::Rotate(this->Rotation)*Matrix4::Translate(this->Position);
    // Keep track of the data so we don't matrix multiply every frame.
    this->oldPosition = this->Position;
    this->oldRotation = this->Rotation;
    this->oldScaling = this->Scaling;
}
