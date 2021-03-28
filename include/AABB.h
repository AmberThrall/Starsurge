#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Sphere.h"
#include "Quad.h"

namespace Starsurge {
    enum AABBCorner {
        FAR_LEFT_BOTTOM,
        FAR_LEFT_TOP,
        FAR_RIGHT_TOP,
        FAR_RIGHT_BOTTOM,
        NEAR_RIGHT_BOTTOM,
        NEAR_LEFT_BOTTOM,
        NEAR_LEFT_TOP,
        NEAR_RIGHT_TOP
    };

    class AABB {
    public:
        AABB();
        AABB(const AABB& copy);
        AABB(float mx, float my, float mz, float Mx, float My, float Mz);
        AABB(Vector3 min, Vector3 max);
        AABB(std::vector<Vector3> points);

        void SetBounds(const AABB& copy);
        void SetBounds(float mx, float my, float mz, float Mx, float My, float Mz);
        void SetBounds(Vector3 min, Vector3 max);
        void SetBounds(std::vector<Vector3> points);

        bool Contains(const Vector3 pt) const;
        bool Contains(const AABB box) const;
        bool Contains(const Sphere sphere) const;
        Vector3 GetCenter() const;
        Vector3 GetSize() const;
        float GetVolume() const;
        Vector3 GetCorner(AABBCorner corner) const;
        std::vector<Vector3> GetAllCorners() const;
        Vector3 GetMinimum() const;
        Vector3 GetMaximum() const;
        bool IsNull() const;
        Vector3 ClosestPoint(const Vector3 point) const;

        std::string ToString();

        static AABB Null();
        static AABB Union(const AABB box, const Vector3 pt);
        static AABB Union(const AABB box, const std::vector<Vector3> pts);
        static AABB Union(const AABB boxA, const AABB boxB);

        AABB Transform(const Matrix<3,3> matrix);
        AABB Transform(const Matrix<4,4> matrix);

        Mesh CreateMesh() const;

        AABB& operator=(const AABB& other) {
            if (this != &other) {
                SetBounds(other);
            }
            return *this;
        }
        AABB& operator+=(const AABB& rhs) {
            if (IsNull() || rhs.IsNull()) {
                return *this;
            }
            this->minimum += rhs.GetMinimum();
            this->maximum += rhs.GetMaximum();
            return *this;
        }
        AABB& operator+=(const Vector3& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum += rhs;
            this->maximum += rhs;
            return *this;
        }
        friend AABB operator+(AABB lhs, const AABB& rhs) { return lhs += rhs; }
        friend AABB operator+(AABB lhs, const Vector3& rhs) { return lhs += rhs; }
        AABB& operator-=(const AABB& rhs) {
            if (IsNull() || rhs.IsNull()) {
                return *this;
            }
            this->minimum -= rhs.GetMinimum();
            this->maximum -= rhs.GetMaximum();
            return *this;
        }
        AABB& operator-=(const Vector3& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum -= rhs;
            this->maximum -= rhs;
            return *this;
        }
        friend AABB operator-(const AABB rhs) { return -1.0f * rhs; }
        friend AABB operator-(AABB lhs, const AABB& rhs) { return lhs -= rhs; }
        friend AABB operator-(AABB lhs, const Vector3& rhs) { return lhs -= rhs; }
        AABB& operator*=(const float& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum *= rhs;
            this->maximum *= rhs;
            return *this;
        }
        friend AABB operator*(AABB lhs, const float rhs) { return lhs *= rhs; }
        friend AABB operator*(const float lhs, AABB rhs) { return rhs *= lhs; }
        AABB& operator/=(const float& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum /= rhs;
            this->maximum /= rhs;
            return *this;
        }
        friend AABB operator/(AABB lhs, const float rhs) { return lhs /= rhs; }
        friend bool operator==(const AABB& lhs, const AABB& rhs) {
            if (lhs.IsNull()) {
                return rhs.IsNull();
            }
            if (rhs.IsNull()) {
                return false;
            }
            return (lhs.GetMinimum() == rhs.GetMinimum() && lhs.GetMaximum() == rhs.GetMaximum());
        }
        friend bool operator!=(const AABB& lhs, const AABB& rhs) { return !(lhs == rhs); }
    private:
        Vector3 minimum, maximum;
        bool isNull;
        void CheckBounds();
    };
};
