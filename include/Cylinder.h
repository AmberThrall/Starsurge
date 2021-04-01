#pragma once
#include "Vector.h"
#include "Mesh.h"
#include "AABB.h"

namespace Starsurge {
    class Cylinder {
    public:
        Cylinder();
        Cylinder(const Cylinder& copy);
        Cylinder(float radius, float height, Vector3 center = Vector3(0,0,0));
        Cylinder(float radius, Vector3 point1, Vector3 point2);

        void SetRadius(float radius);
        float GetRadius() const;

        bool Contains(Vector3 test) const;
        Vector3 ClosestPoint(const Vector3 point) const;
        float Distance(Vector3 p) const;
        float GetVolume() const;
        float GetSurfaceArea() const;
        float GetHeight() const;
        Vector3 GetCenter() const;
        AABB BoundingBox() const;

        std::string ToString(unsigned int ndigits = 3) const;

        Mesh CreateMesh(unsigned int nsegments = 24) const;

        Vector3 Point1, Point2;
    private:
        float Radius;
    };
};
