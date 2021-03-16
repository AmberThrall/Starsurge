#pragma once
#include "Vector.h"

namespace Starsurge {
    class Quaternion {
    public:
        Quaternion();
        Quaternion(float a);
        Quaternion(Vector3 t_vector);
        Quaternion(const Quaternion& other);
        Quaternion(float a, float b, float c, float d);
        Quaternion(float t_scalar, Vector3 t_vector);

        bool IsReal();
        bool IsPure();

        std::string ToString();

        Quaternion Identity();
        Quaternion Inverse();
        Quaternion Conjugate();
        Quaternion ConjugateBy(Quaternion q);
        Vector3 Rotate(Vector3 v);
        Vector3 Axis();
        float Angle();

        static float Dot(Quaternion p, Quaternion q);
        static float AngleBetween(Quaternion p, Quaternion q);
        static Quaternion FromEulerAngles(Vector3 euler);

        float Norm();
        float Magnitude();
        Quaternion Normalize();

        // Operators:
        Quaternion& operator=(const Quaternion& other) {
            if (this != &other) {
                this->scalar = other.scalar;
                this->vector = other.vector;
            }
            return *this;
        }
        Quaternion& operator+=(const Quaternion& rhs) {
            this->scalar += rhs.scalar;
            this->vector += rhs.vector;
            return *this;
        }
        friend Quaternion operator+(Quaternion lhs, const Quaternion& rhs) { return lhs += rhs; }
        Quaternion& operator-=(const Quaternion& rhs) {
            this->scalar -= rhs.scalar;
            this->vector -= rhs.vector;
            return *this;
        }
        friend Quaternion operator-(Quaternion lhs, const Quaternion& rhs) { return lhs -= rhs; }
        Quaternion& operator*=(const Quaternion& rhs) {
            this->scalar = this->scalar*rhs.scalar - Vector3::Dot(this->vector, rhs.vector);
            this->vector = this->scalar*rhs.vector + rhs.scalar*this->vector + Vector3::CrossProduct(this->vector, rhs.vector);
            return *this;
        }
        friend Quaternion operator*(Quaternion lhs, const Quaternion& rhs) { return lhs *= rhs; }
        Quaternion& operator*=(const float& rhs) {
            this->scalar *= rhs;
            this->vector *= rhs;
            return *this;
        }
        friend Quaternion operator*(Quaternion lhs, const float& rhs) { return lhs *= rhs; }
        friend Quaternion operator*(const float& lhs, Quaternion rhs) { return rhs *= lhs; }
        Quaternion& operator/=(const float& rhs) {
            this->scalar *= rhs;
            this->vector *= rhs;
            return *this;
        }
        friend Quaternion operator/(Quaternion lhs, const float& rhs) { return lhs /= rhs; }
        friend Quaternion operator/(const float& lhs, Quaternion rhs) { return rhs /= lhs; }
        friend bool operator==(const Quaternion& lhs, const Quaternion& rhs) {
            return (lhs.scalar == rhs.scalar && lhs.vector == rhs.vector);
        }
        friend bool operator!=(const Quaternion& lhs, const Quaternion& rhs) { return !(lhs == rhs); }

        float scalar;
        Vector3 vector;
    };
}
