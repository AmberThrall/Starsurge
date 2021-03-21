#pragma once
#include "Vector.h"
#include "EulerAngles.h"

namespace Starsurge {
    // Predefine classes due to circular inclusion.
    class EulerAngles;
    template<size_t M, size_t N> class Matrix;

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

        Quaternion Inverse();
        Quaternion Conjugate();
        Quaternion ConjugateBy(Quaternion q);
        Vector3 Rotate(Vector3 v);
        Vector3 Axis();
        float Angle();

        static Quaternion Lerp(Quaternion start, Quaternion end, float t);
        static Quaternion Identity();
        static float Dot(Quaternion p, Quaternion q);
        static float AngleBetween(Quaternion p, Quaternion q);
        static Quaternion FromEulerAngles(EulerAngles euler);
        static Quaternion FromAxisAngle(float theta, Vector3 axis);
        static Quaternion FromMatrix(Matrix<3,3> m);

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
            float s = this->scalar*rhs.scalar - Vector3::Dot(this->vector, rhs.vector);
            Vector3 v = this->scalar*rhs.vector + rhs.scalar*this->vector + Vector3::CrossProduct(this->vector, rhs.vector);
            this->scalar = s;
            this->vector = v;
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
