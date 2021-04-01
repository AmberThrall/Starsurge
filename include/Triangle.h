#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Line.h"
#include "Mesh.h"
#include "Plane.h"

namespace Starsurge {
    class Triangle {
    public:
        Triangle();
        Triangle(const Triangle& other);
        Triangle(Vector3 p1, Vector3 p2, Vector3 p3);

        void SetVertices(const Triangle& other);
        void SetVertices(Vector3 p1, Vector3 p2, Vector3 p3);

        Vector3 GetVertex(unsigned int i) const;
        bool Contains(Vector3 point) const;
        Vector3 ClosestPoint(const Vector3 point) const;
        float Distance(Vector3 p) const;
        float GetAngle(unsigned int i) const;
        Line GetEdge(unsigned int i, unsigned int j) const;
        std::vector<Line> GetAllEdges() const;
        Vector3 GetCenter() const;
        Vector3 GetNormal() const;
        float GetArea() const;
        float GetPerimeter() const;
        Plane ExtendToPlane() const;

        void FlipNormal();

        std::string ToString(unsigned int ndigits = 3) const;

        Mesh CreateMesh() const;
    private:
        Vector3 point1, point2, point3;
        Vector3 normal;
    };
}
