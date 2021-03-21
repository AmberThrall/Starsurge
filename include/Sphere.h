#pragma once
#include "Vector.h"

namespace Starsurge {
    class Sphere {
    public:
        Sphere();
        Sphere(const Sphere& copy);
        Sphere(Vector3 p, float r);

        bool Contains(const Vector3 pt) const;
        float GetVolume() const;
        float GetSurfaceArea() const;
        Vector3 ClosestPoint(const Vector3 point) const;

        std::string ToString() const;

        Vector3 position;
        float radius;
    };
};
