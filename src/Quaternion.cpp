#include "../include/Quaternion.h"
#include "../include/Utils.h"

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
    ret += std::to_string(this->scalar);
    if (this->vector.x < 0) ret += " - ";
    else ret += " + ";
    ret += std::to_string(std::abs(this->vector.x)) + "i";
    if (this->vector.y < 0) ret += " - ";
    else ret += " + ";
    ret += std::to_string(std::abs(this->vector.y)) + "j";
    if (this->vector.z < 0) ret += " - ";
    else ret += " + ";
    ret += std::to_string(std::abs(this->vector.z)) + "k";
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

Starsurge::Vector3 Starsurge::Quaternion::Axis() {
    double s = std::sqrt(1-this->scalar*this->scalar);
    if (s < 0.000001) {
        return this->vector;
    }
    return this->vector / s;
}

float Starsurge::Quaternion::Angle() {
    return Degrees(2*std::acos(this->scalar));
}

float Starsurge::Quaternion::Dot(Quaternion p, Quaternion q) {
    return p.scalar*q.scalar + Vector3::Dot(p.vector, q.vector);
}

float Starsurge::Quaternion::AngleBetween(Quaternion p, Quaternion q) {
    return std::acos(Quaternion::Dot(p,q) / (p.Norm() * q.Norm()));
}

Starsurge::Quaternion Starsurge::Quaternion::FromEulerAngles(EulerAngles euler) {
    float c1 = std::cos(Radians(euler.x) / 2);
    float c2 = std::cos(Radians(euler.y) / 2);
    float c3 = std::cos(Radians(euler.z) / 2);
    float s1 = std::sin(Radians(euler.x) / 2);
    float s2 = std::sin(Radians(euler.y) / 2);
    float s3 = std::sin(Radians(euler.z) / 2);

    float x, y, z, w;
    switch (euler.GetOrder()) {
        case XYZ:
            x = s1 * c2 * c3 + c1 * s2 * s3;
            y = c1 * s2 * c3 - s1 * c2 * s3;
            z = c1 * c2 * s3 + s1 * s2 * c3;
            w = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        case YZX:
            x = s1 * c2 * c3 + c1 * s2 * s3;
            y = c1 * s2 * c3 + s1 * c2 * s3;
            z = c1 * c2 * s3 - s1 * s2 * c3;
            w = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        case ZXY:
            x = s1 * c2 * c3 + c1 * s2 * s3;
            y = c1 * s2 * c3 + s1 * c2 * s3;
            z = c1 * c2 * s3 - s1 * s2 * c3;
            w = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        case XZY:
            x = s1 * c2 * c3 - c1 * s2 * s3;
            y = c1 * s2 * c3 - s1 * c2 * s3;
            z = c1 * c2 * s3 + s1 * s2 * c3;
            w = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        case YXZ:
            x = s1 * c2 * c3 + c1 * s2 * s3;
            y = c1 * s2 * c3 - s1 * c2 * s3;
            z = c1 * c2 * s3 - s1 * s2 * c3;
            w = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        case ZYX:
            x = s1 * c2 * c3 - c1 * s2 * s3;
            y = c1 * s2 * c3 + s1 * c2 * s3;
            z = c1 * c2 * s3 - s1 * s2 * c3;
            w = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        default:
            throw "Unknown order.";
    };

    return Quaternion(w, x, y, z);
}

Starsurge::Quaternion Starsurge::Quaternion::FromAxisAngle(float theta, Vector3 axis) {
    axis.Normalize();
    return Quaternion(std::cos(theta/2), axis*std::sin(theta / 2));
}

Starsurge::Quaternion Starsurge::Quaternion::FromMatrix(Matrix3 m) {
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
    float tr = m.Trace();
    if (tr > 0) {
        float s = std::sqrt(tr + 1.0) * 2;
        return Quaternion(s/4, (m(2,1) - m(1,2)) / s, (m(0,2)-m(2,0)) / s, (m(1,0)-m(0,1)) / s);
    }
    else if (m(0,0) > m(1,1) && m(0,0) > m(2,2)) {
        float s = std::sqrt(1.0 + m(0,0) - m(1,1) - m(2,2)) * 2;
        return Quaternion((m(2,1)-m(1,2)) / s, s / 4, (m(0,1)+m(1,0)) / s, (m(0,2)+m(2,0)) / s);
    }
    else if (m(1,1) > m(2,2)) {
        float s = std::sqrt(1.0 + m(1,1) - m(0,0) - m(2,2)) * 2;
        return Quaternion((m(0,2)-m(2,0)) / s, (m(0,1)+m(1,0)) / s, s / 4, (m(1,2) + m(2,1)) / s);
    }
    else {
        float s = std::sqrt(1.0 + m(2,2) - m(0,0) - m(1,1)) * 2;
        return Quaternion((m(1,0)-m(0,1)) / s, (m(0,2) + m(2,0)) / s, (m(1,2) + m(2,1)) / s, s / 4);
    }
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
