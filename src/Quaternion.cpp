#include "../include/Quaternion.h"

Starsurge::Quaternion::Quaternion() {
    this->scalar = 0;
    this->vector = Vector3(0,0,0);
}

Starsurge::Quaternion::Quaternion(float a) {
    this->scalar = a;
    this->vector = Vector3(0,0,0);
}

Starsurge::Quaternion::Quaternion(Vector3 t_vector) {
    this->scalar = 0;
    this->vector = t_vector;
}

Starsurge::Quaternion::Quaternion(const Quaternion& other) {
    this->scalar = other.scalar;
    this->vector = other.vector;
}

Starsurge::Quaternion::Quaternion(float a, float b, float c, float d) {
    this->scalar = a;
    this->vector = Vector3(b, c, d);

}

Starsurge::Quaternion::Quaternion(float t_scalar, Vector3 t_vector) {
    this->scalar = t_scalar;
    this->vector = t_vector;
}

bool Starsurge::Quaternion::IsReal() {
    return (this->vector == Vector3(0,0,0));
}

bool Starsurge::Quaternion::IsPure() {
    return (this->scalar == 0);
}

std::string Starsurge::Quaternion::ToString() {
    std::string ret = "";
    ret += std::to_string(this->scalar) + " + ";
    ret += std::to_string(this->vector.x) + "i + ";
    ret += std::to_string(this->vector.y) + "j + ";
    ret += std::to_string(this->vector.z) + "k";
    return ret;
}

Starsurge::Quaternion Starsurge::Quaternion::Identity() {
    Quaternion ret(1,0,0,0);
    return ret;
}

Starsurge::Quaternion Starsurge::Quaternion::Inverse() {
    Quaternion ret = Conjugate();
    float mag = std::pow(Norm(), 2);
    return ret / mag;
}

Starsurge::Quaternion Starsurge::Quaternion::Conjugate() {
    Quaternion ret(this->scalar, -1.0f * this->vector);
    return ret;
}

Starsurge::Quaternion Starsurge::Quaternion::ConjugateBy(Quaternion q) {
    Quaternion p(*this);
    return q*p*q.Inverse();
}

Starsurge::Vector3 Starsurge::Quaternion::Rotate(Vector3 v) {
    Quaternion p(v);
    return (p.ConjugateBy(*this)).vector;
}

Starsurge::Vector3 Starsurge::Quaternion::Axis() {
    Quaternion copy(*this);
    copy.scalar = 0;
    return copy.vector / copy.Norm();
}

float Starsurge::Quaternion::Angle() {
    Quaternion copy(*this);
    copy.scalar = 0;
    return 2*std::atan2(copy.Norm(), this->scalar);
}

float Starsurge::Quaternion::Dot(Quaternion p, Quaternion q) {
    return p.scalar*q.scalar + Vector3::Dot(p.vector, q.vector);
}

float Starsurge::Quaternion::AngleBetween(Quaternion p, Quaternion q) {
    return std::acos(Quaternion::Dot(p,q) / (p.Norm() * q.Norm()));
}

Starsurge::Quaternion Starsurge::Quaternion::FromEulerAngles(Vector3 euler) {
    float cy = std::cos(euler.x / 2);
    float sy = std::sin(euler.x / 2);
    float cp = std::cos(euler.y / 2);
    float sp = std::sin(euler.y / 2);
    float cr = std::cos(euler.z / 2);
    float sr = std::sin(euler.z / 2);

    Quaternion q(0, euler);
    return q;
}

float Starsurge::Quaternion::Norm() {
    Quaternion q(*this);
    return std::sqrt((q*q.Conjugate()).scalar);
}

float Starsurge::Quaternion::Magnitude() {
    return Norm();
}

Starsurge::Quaternion Starsurge::Quaternion::Normalize() {
    Quaternion q(*this);
    float norm = Norm();
    return q / norm;
}
