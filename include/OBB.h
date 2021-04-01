#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Line.h"
#include "Quad.h"
#include "AABB.h"

namespace Starsurge {
    // Predefine classes due to circular depenendencies
    class AABB;

    enum BoxCorner {
        FAR_LEFT_BOTTOM,
        FAR_LEFT_TOP,
        FAR_RIGHT_TOP,
        FAR_RIGHT_BOTTOM,
        NEAR_RIGHT_BOTTOM,
        NEAR_LEFT_BOTTOM,
        NEAR_LEFT_TOP,
        NEAR_RIGHT_TOP
    };

    enum BoxFace {
        LEFT_FACE, TOP_FACE, RIGHT_FACE, BOTTOM_FACE, NEAR_FACE, FAR_FACE
    };

    class OBB {
    public:
        OBB();
        OBB(const OBB& copy);
        OBB(float size, Vector3 center = Vector3(0,0,0), Vector3 axis = Vector3(1,0,0));
        OBB(float size, Vector3 center, Vector3 axis1, Vector3 axis2);
        OBB(float size, Vector3 center, Vector3 axis1, Vector3 axis2, Vector3 axis3);
        OBB(Vector3 size, Vector3 center = Vector3(0,0,0), Vector3 axis = Vector3(1,0,0));
        OBB(Vector3 size, Vector3 center, Vector3 axis1, Vector3 axis2);
        OBB(Vector3 size, Vector3 center, Vector3 axis1, Vector3 axis2, Vector3 axis3);

        void SetSize(float size);
        void SetSize(Vector3 size);
        void SetOrientation(Vector3 axis);
        void SetOrientation(Vector3 axis1, Vector3 axis2);
        void SetOrientation(Vector3 axis1, Vector3 axis2, Vector3 axis3);
        void SetOrientation(Matrix3 m);

        bool IsNull() const;
        bool Contains(const Vector3 pt) const;
        Vector3 ClosestPoint(const Vector3 point) const;
        float Distance(const Vector3 point) const;
        Vector3 GetCorner(BoxCorner corner) const;
        std::vector<Vector3> GetAllCorners() const;
        Line GetEdge(BoxCorner corner1, BoxCorner corner2) const;
        std::vector<Line> GetAllEdges() const;
        Quad GetFace(BoxFace face) const;
        std::vector<Quad> GetAllFaces() const;
        Vector3 GetSize() const;
        Vector3 GetExtents() const;
        float GetVolume() const;
        Vector3 GetAxis(unsigned int i) const;
        Matrix3 GetOrientation() const;
        AABB FitAABB() const;

        void SetNull();
        void Expand(float amount);
        void Expand(Vector3 amount);

        std::string ToString(unsigned int ndigits = 3);

        static OBB Null();

        void Transform(const Matrix<3,3> matrix);
        void Transform(const Matrix<4,4> matrix);

        Mesh CreateMesh() const;

        Vector3 Center;
    private:
        void CheckParams();

        Vector3 u, v, w;
        Vector3 extents;
        bool isNull;
    };
};
