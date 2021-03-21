#pragma once
#include "Vector.h"
#include "Matrix.h"

namespace Starsurge {
    // Predefine classes due to circular inclusion.
    template <size_t M, size_t N> class Matrix;

    // ax + by + cz + d = 0
    class Plane {
    public:
        Plane();
        Plane(const Plane& copy);
        Plane(float a, float b, float c, float d);
        Plane(Vector3 n, float d);
        Plane(Vector3 n, Vector3 p);
        Plane(Vector3 p1, Vector3 p2, Vector3 p3);

        bool Above(Vector3 test);
        bool Below(Vector3 test);
        bool Contains(Vector3 test);
        void Normalize();
        Vector3 ClosestPoint(const Vector3 point);
        Vector3 GetNormal() const;

        std::string ToString() const;

        static Plane YZ();
        static Plane XZ();
        static Plane XY();

        float Distance(Vector3 p) const;
        Vector3 Project(const Vector3 v) const;
        Plane Transform(Matrix<4,4> matrix);

        float a, b, c, d;
    };
};
