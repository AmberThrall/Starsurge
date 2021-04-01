#pragma once
#include "Vector.h"
#include "Mesh.h"
#include "AABB.h"
#include "Plane.h"

namespace Starsurge {
    // Cone(V,D,theta,hmin,hmax)
    //  - V = Origin
    //  - D = Direction
    //  - theta = Angle (in Degrees) between cone's sides.
    //  - hmin = Minimum distance from V before the cone begins.
    //  - hmax = Maximum distance from V before the cone ends.
    class Cone {
    public:
        Cone();
        Cone(const Cone& copy);
        Cone(Vector3 origin, Vector3 dir);
        Cone(float r, float height, Vector3 origin = Vector3(0,0,0), Vector3 dir = Vector3(0,1,0));
        Cone(float hmin, float hmax, float theta, Vector3 origin = Vector3(0,0,0), Vector3 dir = Vector3(0,1,0));

        void SetHeight(float height);
        void SetFrustum(float hmin, float hmax);
        void SetRadius(float r);
        void SetRadii(float r1, float r2);
        void SetAngle(float theta);
        void SetDirection(Vector3 dir);

        bool IsInfinite() const;
        bool IsNull() const;
        bool Contains(Vector3 test) const;
        Vector3 ClosestPoint(const Vector3 point) const;
        float Distance(Vector3 p) const;
        float GetVolume() const;
        float GetSurfaceArea() const;
        float GetRadius(float h) const;
        float GetHeight() const;
        float GetSlantHeight() const;
        Vector3 GetBase() const;
        Vector3 GetPeak() const;
        Vector3 GetCenter() const;
        Vector3 GetDirection() const;
        float GetAngle() const;
        float GetHMin() const;
        float GetHMax() const;

        AABB BoundingBox() const;

        std::string ToString(unsigned int ndigits = 3) const;

        Mesh CreateMesh(unsigned int nsegments = 24) const;

        Vector3 Origin;
    private:
        Vector3 Direction;
        float HMin, HMax, Angle;
    };
};
