#pragma once
#include "Vector.h"
#include "Mesh.h"

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

        Mesh CreateMesh(unsigned int nsubdivisions) const;

        Vector3 position;
        float radius;
    private:
        void Subdivide(std::vector<Vector3> & vertices, std::vector<unsigned int> & indices) const;
    };
};
