#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Line.h"
#include "Sphere.h"

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

    enum AABBFace {
        LEFT_FACE, TOP_FACE, RIGHT_FACE, BOTTOM_FACE, NEAR_FACE, FAR_FACE
    };

    struct Quad {
        Quad() {}
        Quad(Vector3 _v1, Vector3 _v2, Vector3 _v3, Vector3 _v4, Vector3 _normal) : v1(_v1), v2(_v2), v3(_v3), v4(_v4), normal(_normal) {}
        Vector3 v1, v2, v3, v4;
        Vector3 normal;
    };

    class AABB {
    public:
        AABB();
        AABB(const AABB& copy);
        AABB(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax);
        AABB(Vector3 min, Vector3 max);
        AABB(std::vector<Vector3> points);

        void SetBounds(const AABB& copy);
        void SetBounds(float mx, float my, float mz, float Mx, float My, float Mz);
        void SetBounds(Vector3 min, Vector3 max);
        void SetBounds(std::vector<Vector3> points);

        bool Contains(const Vector3 pt) const;
        bool Contains(const AABB box) const;
        Vector3 GetCenter() const;
        Vector3 GetSize() const;
        float GetVolume() const;
        Vector3 GetCorner(AABBCorner corner) const;
        std::vector<Vector3> GetAllCorners() const;
        Line GetEdge(AABBCorner corner1, AABBCorner corner2) const;
        std::vector<Line> GetAllEdges() const;
        Quad GetFace(AABBFace face) const;
        std::vector<Quad> GetAllFaces() const;
        Vector3 GetMinimum() const;
        Vector3 GetMaximum() const;
        bool IsNull() const;
        Vector3 ClosestPoint(const Vector3 point) const;

        void Expand(float amount);
        void Expand(Vector3 amount);
        void SetNull();

        std::string ToString(unsigned int ndigits = 3);

        static AABB Null();
        void Union(const Vector3 pt);
        void Union(const std::vector<Vector3> pts);
        void Union(const AABB boxB);
        void Subtract(const Vector3 pt);
        void Subtract(const std::vector<Vector3> pts);
        void Subtract(const AABB boxB);
        void Intersection(const AABB boxB);

        void Transform(const Matrix<3,3> matrix);
        void Transform(const Matrix<4,4> matrix);

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
            CheckBounds(false);
            return *this;
        }
        AABB& operator+=(const Vector3& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum += rhs;
            this->maximum += rhs;
            CheckBounds(false);
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
            CheckBounds(false);
            return *this;
        }
        AABB& operator-=(const Vector3& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum -= rhs;
            this->maximum -= rhs;
            CheckBounds(false);
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
            CheckBounds(false);
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
            CheckBounds(false);
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
        void CheckBounds(bool raiseError = true);
    };
};
