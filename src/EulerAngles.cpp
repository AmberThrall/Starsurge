#include "../include/EulerAngles.h"

Starsurge::EulerAngles::EulerAngles(float all, EulerAngleOrders order) : Vector3(all) {
    this->order = order;
}
Starsurge::EulerAngles::EulerAngles(const Vector3& v, EulerAngleOrders order) : Vector3(v) {
    this->order = order;
}
Starsurge::EulerAngles::EulerAngles(const EulerAngles& other) : Vector3((Vector3)other) {
    this->order = other.GetOrder();
}
Starsurge::EulerAngles::EulerAngles(float t_data[3], EulerAngleOrders order) : Vector3(t_data) {
    this->order = order;
}
Starsurge::EulerAngles::EulerAngles(float x, float y, float z, EulerAngleOrders order) : Vector3(x,y,z) {
    this->order = order;
}

Starsurge::EulerAngleOrders Starsurge::EulerAngles::GetOrder() const {
    return this->order;
}
void Starsurge::EulerAngles::SetOrder(EulerAngleOrders order) {
    if (this->order == order)
        return;

    *this = FromMatrix(Matrix3::Rotate(*this), order);
    this->order = order;
}

std::string Starsurge::EulerAngles::ToString() {
    std::string ret = "EulerAngles { ";
    ret += ((Vector3)*this).ToString();
    ret += ", ";
    switch (this->order) {
        case XYZ: ret += "XYZ"; break;
        case YZX: ret += "YZX"; break;
        case ZXY: ret += "ZXY"; break;
        case XZY: ret += "XZY"; break;
        case YXZ: ret += "YXZ"; break;
        case ZYX: ret += "ZYX"; break;
        default:
            ret += "???";
    };
    ret += " }";
    return ret;
}

Starsurge::Vector3 Starsurge::EulerAngles::Rotate(Vector3 v) {
    Matrix3 mat = Matrix3::Rotate(*this);
    return mat*v;
}

Starsurge::EulerAngles Starsurge::EulerAngles::FromMatrix(Matrix3 m, EulerAngleOrders order) {
    // https://github.com/mrdoob/three.js/blob/dev/src/math/Euler.js
    float x, y, z;

    const float m11 = m(0,0), m12 = m(0,1), m13 = m(0,2);
    const float m21 = m(1,0), m22 = m(1,1), m23 = m(1,2);
    const float m31 = m(2,0), m32 = m(2,1), m33 = m(2,2);

    switch (order) {
        case XYZ:
              y = std::asin(Starsurge::Clamp(m13, -1, 1));
              if (std::abs(m13) < 0.9999999) { // Check for singularity (cy == 0).
                x = std::atan2(-m23, m33);
                z = std::atan2(-m12, m11);
              }
              else {
                x = std::atan2(m32, m22);
                z = 0;
              }
            break;
        case YXZ:
            x = std::asin(-Starsurge::Clamp(m23, -1, 1));
            if (std::abs(m23) < 0.9999999) {
                y = std::atan2(m13, m33);
                z = std::atan2(m21, m22);
            }
            else {
                y = std::atan2(-m31, m11);
                z = 0;
            }
            break;
        case ZXY:
            x = std::asin(Starsurge::Clamp(m32, -1, 1));
            if (std::abs(m32) < 0.9999999) {
                y = std::atan2(-m31, m33);
                z = std::atan2(-m12, m22);
            }
            else {
                y = 0;
                z = std::atan2(m21, m11);
            }
            break;
        case ZYX:
            y = std::asin(-Starsurge::Clamp(m31, -1, 1));
            if (std::abs(m31) < 0.9999999) {
                x = std::atan2(m32, m33);
                z = std::atan2(m21, m11);
            }
            else {
                x = 0;
                z = std::atan2(-m12, m22);
            }
            break;
        case YZX:
            z = std::asin(Starsurge::Clamp(m21, -1, 1));
            if (std::abs(m21) < 0.9999999) {
                x = std::atan2(-m23, m22);
                y = std::atan2(-m31, m11);
            }
            else {
                x = 0;
                y = std::atan2(m13, m33);
            }
            break;
        case XZY:
            z = std::asin(-Starsurge::Clamp(m12, -1, 1));
            if (std::abs(m12) < 0.9999999) {
                x = std::atan2(m32, m22);
                y = std::atan2(m13, m11);
            }
            else {
                x = std::atan2(-m23, m33);
                y = 0;
            }
            break;
        default:
            throw "Unknown order.";
    };

    return EulerAngles(Degrees(x), Degrees(y), Degrees(z), order);
}

Starsurge::EulerAngles Starsurge::EulerAngles::FromQuaternion(Quaternion q, EulerAngleOrders order) {
    return EulerAngles::FromMatrix(Matrix3::Rotate(q), order);
}

Starsurge::EulerAngles Starsurge::EulerAngles::FromAxisAngle(float theta, Vector3 axis, EulerAngleOrders order) {
    return EulerAngles::FromMatrix(Matrix3::Rotate(theta, axis), order);
}
