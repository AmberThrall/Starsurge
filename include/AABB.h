#pragma once
#include "Vector.h"
#include "Matrix.h"
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
    private:
        Vector3 minimum, maximum;
        bool isNull;
        void CheckBounds();
    };
};
