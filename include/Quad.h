#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Line.h"
#include "Mesh.h"
#include "Plane.h"
#include "Triangle.h"

namespace Starsurge {
    class Quad {
    public:
        Quad();
        Quad(const Quad& other);
        Quad(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4);

        void SetVertices(const Quad& other);
        void SetVertices(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4);

        Vector3 GetVertex(unsigned int i) const;
        bool Contains(Vector3 point) const;
        Vector3 ClosestPoint(const Vector3 point) const;
        float Distance(Vector3 p) const;
        Line GetEdge(unsigned int i, unsigned int j) const;
        std::vector<Line> GetAllEdges() const;
        Vector3 GetCenter() const;
        Vector3 GetNormal() const;
        float GetArea() const;
        float GetPerimeter() const;
        Plane ExtendToPlane() const;

        void FlipNormal();

        std::vector<Triangle> Triangulate() const;
        std::string ToString(unsigned int ndigits = 3) const;

        Mesh CreateMesh() const;
    private:
        Vector3 point1, point2, point3, point4;
        Vector3 normal;
    };
}
