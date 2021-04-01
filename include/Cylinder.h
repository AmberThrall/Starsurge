#pragma once
#include "Vector.h"
#include "Mesh.h"
#include "AABB.h"

namespace Starsurge {
    class Cylinder {
    public:
        Cylinder();
        Cylinder(const Cylinder& copy);
        Cylinder(float radius, Vector3 center = Vector3(0,0,0), Vector3 dir = Vector3(0,1,0));
        Cylinder(float radius, float height, Vector3 center = Vector3(0,0,0), Vector3 dir = Vector3(0,1,0));

        void SetHeight(float h);
        void SetRadius(float r);
        void SetDirection(Vector3 dir);

        bool IsInfinite() const;
        bool Contains(Vector3 test) const;
        Vector3 ClosestPoint(const Vector3 point) const;
        float Distance(Vector3 p) const;
        float GetVolume() const;
        float GetSurfaceArea() const;
        float GetHeight() const;
        float GetRadius() const;
        Vector3 GetDirection() const;
        Vector3 GetTop() const;
        Vector3 GetBottom() const;
        AABB BoundingBox() const;

        std::string ToString(unsigned int ndigits = 3) const;

        Mesh CreateMesh(unsigned int nsegments = 24) const;

        Vector3 Origin;
    private:
        float radius, height;
        Vector3 direction;
    };
};
