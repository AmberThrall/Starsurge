#pragma once
#include <vector>
#include "Vector.h"
#include "Matrix.h"
#include "Line2D.h"

namespace Starsurge {
    class Polygon {
    public:
        Polygon();
        Polygon(const Polygon& copy);
        Polygon(std::vector<Vector2> verts);
        template <typename... Ts>
        Polygon(Ts const&... ts) {
            std::vector<Vector2> vertices{ts...};
            AddVertices(vertices);
        }

        // Vertex manipulation
        void AddVertices(Vector2 v);
        void AddVertices(std::vector<Vector2> verts);
        template <typename...Ts>
        void AddVertices(Vector2 a, Ts&&... ts) {
            AddVertices(a);
            return AddVertices(std::forward<Ts>(ts)...);
        }
        void InsertVertices(size_t i, Vector2 v);
        void InsertVertices(size_t i, std::vector<Vector2> verts);
        template <typename...Ts>
        void InsertVertices(size_t i, Vector2 a, Ts&&... ts) {
            if (i < 0 || i > vertices.size())
                return;
            InsertVertices(i, a);
            return InsertVertices(i+1, std::forward<Ts>(ts)...);
        }
        void RemoveVertices(size_t i);
        void RemoveVertices(std::vector<size_t> indices);
        template <typename...Ts>
        void RemoveVertices(Ts const&... ts) {
            std::vector<size_t> indices{ts...};
            RemoveVertices(indices);
        }

        // Getters and Testers
        bool IsNull() const;
        bool IsConvex() const;
        bool IsSimple() const;
        bool IsRegular() const;
        bool Contains() const;
        size_t NumSides() const;
        float GetInteriorAngle(size_t i) const;
        float GetExteriorAngle(size_t i) const;
        float GetArea() const;
        float GetPerimeter() const;
        Vector2 GetCenter() const;
        Vector2 GetVertex(size_t i) const;
        std::vector<Vector2> GetAllVertices() const;
        Line2D GetEdge(size_t i, size_t j) const;
        std::vector<Line2D> GetAllEdges() const;

        // Boolean operations
        void Union(Polygon other);
        void Subtract(Polygon other);
        void Intersection(Polygon other);
        void Difference(Polygon other);

        // Special
        std::string ToString(unsigned int ndigits = 3) const;
        std::vector<Polygon> Triangulate() const;

        // Static Constructors.
        static Polygon Null();
        static Polygon ConvexRegular(size_t n, float radius);
        static Polygon Star(size_t m, size_t n, float radius);
        static Polygon ConvexHull(std::vector<Vector2> points);
    private:
        std::vector<Vector2> vertices;
    };
}
