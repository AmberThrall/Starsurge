#pragma once
#include "Vector.h"

namespace Starsurge {
    // L(t) = p+tv, t >= 0
    class Ray {
    public:
        Ray(Vector3 p, Vector3 v);

        bool Contains(Vector3 test) const;
        Vector3 GetPoint(float t) const;
        float Distance(Vector3 p) const;
        Vector3 ClosestPoint(const Vector3 point) const;
        float GetT(Vector3 point) const;

        std::string ToString() const;

        Vector3 operator*(float t) const {
            return GetPoint(t);
        }

        Vector3 origin;
        Vector3 direction;
    };
};
