#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Mesh.h"

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

        bool IsAbove(Vector3 test);
        bool IsBelow(Vector3 test);
        bool Contains(Vector3 test);
        void Normalize();
        Vector3 ClosestPoint(const Vector3 point);
        Vector3 GetNormal() const;

        std::string ToString(unsigned int ndigits = 6) const;

        static Plane YZ();
        static Plane XZ();
        static Plane XY();

        float Distance(Vector3 p) const;
        Vector3 Project(const Vector3 v) const;
        Plane Transform(Matrix<4,4> matrix);

        template<size_t M,size_t N>
        Mesh CreateMesh(Vector<M> x, Vector<N> y) const {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;

            // Build the vertices.
            for (unsigned int r = 0; r < x.Size(); r++) {
                for (unsigned int c = 0; c < y.Size(); c++) {
                    float _x, _y, _z;
                    if (this->a != 0 && this->b == 0 && this->c == 0) { // ax + d = 0
                        _x = -this->d / this->a;
                        _y = x[r];
                        _z = y[c];
                    }
                    else if (this->a == 0 && this->b != 0 && this->c == 0) { // by + d = 0
                        _x = x[r];
                        _y = -this->d / this->b;
                        _z = y[c];
                    }
                    else if (this->a == 0 && this->b == 0 && this->c != 0) { // cz + d = 0
                        _x = x[r];
                        _y = y[c];
                        _z = -this->d / this->c;
                    }
                    else if (this->a != 0 && this->b != 0 && this->c == 0) { // ax + by + d = 0
                        _x = x[r];
                        _y = (-this->d-_x) / this->b;
                        _z = y[c];
                    }
                    else if (this->a != 0 && this->b == 0 && this->c != 0) { // ax + cz + d = 0
                        _x = x[r];
                        _y = y[c];
                        _z = (-this->d-_x) / this->c;
                    }
                    else if (this->a == 0 && this->b != 0 && this->c != 0) { // by + cz + d = 0
                        _x = x[r];
                        _y = y[c];
                        _z = (-this->d-_y) / this->c;
                    }
                    else if (this->a != 0 && this->b != 0 && this->c != 0) { // ax + by + cz + d = 0
                        _x = x[r];
                        _y = y[c];
                        _z = (-this->d-_x-_y) / this->c;
                    }
                    else {
                        Error("Invalid plane! Cannot create mesh.");
                        return Mesh(vertices, indices);
                    }
                    Vertex v;
                    v.Position = Vector3(_x, _y, _z);
                    v.Normal = GetNormal();
                    v.UV = Vector2(r / (x.Size()-1),c / (y.Size()-1));
                    v.Color = Colors::WHITE;
                    vertices.push_back(v);
                }
            }

            // Build the indices
            for (unsigned int r = 0; r < x.Size()-1; r++) {
                for (unsigned int c = 0; c < y.Size()-1; c++) {
                    indices.push_back(r*y.Size()+c);
                    indices.push_back(r*y.Size()+c+1);
                    indices.push_back((r+1)*y.Size()+c+1);
                    indices.push_back(r*y.Size()+c);
                    indices.push_back((r+1)*y.Size()+c);
                    indices.push_back((r+1)*y.Size()+c+1);
                }
            }

            return Mesh(vertices, indices);
        }

        float a, b, c, d;
    };
};
