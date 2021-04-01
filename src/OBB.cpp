#include <map>
#include "../include/OBB.h"
#include "../include/AABB.h"
#include "../include/Utils.h"

Starsurge::OBB::OBB() {
    SetNull();
}
Starsurge::OBB::OBB(const OBB& copy) {
    this->extents = copy.GetExtents();
    this->u = copy.GetAxis(0);
    this->v = copy.GetAxis(1);
    this->w = copy.GetAxis(2);
    this->Center = copy.Center;
    this->isNull = copy.IsNull();
}
Starsurge::OBB::OBB(float size, Vector3 center, Vector3 axis) {
    SetSize(size);
    SetOrientation(axis);
    this->Center = center;
}
Starsurge::OBB::OBB(float size, Vector3 center, Vector3 axis1, Vector3 axis2) {
    SetSize(size);
    SetOrientation(axis1, axis2);
    this->Center = center;
}
Starsurge::OBB::OBB(float size, Vector3 center, Vector3 axis1, Vector3 axis2, Vector3 axis3) {
    SetSize(size);
    SetOrientation(axis1, axis2, axis3);
    this->Center = center;
}
Starsurge::OBB::OBB(Vector3 size, Vector3 center, Vector3 axis) {
    SetSize(size);
    SetOrientation(axis);
    this->Center = center;
}
Starsurge::OBB::OBB(Vector3 size, Vector3 center, Vector3 axis1, Vector3 axis2) {
    SetSize(size);
    SetOrientation(axis1, axis2);
    this->Center = center;
}
Starsurge::OBB::OBB(Vector3 size, Vector3 center, Vector3 axis1, Vector3 axis2, Vector3 axis3) {
    SetSize(size);
    SetOrientation(axis1, axis2, axis3);
    this->Center = center;
}

void Starsurge::OBB::SetSize(float size) {
    SetSize(Vector3(size, size, size));
}
void Starsurge::OBB::SetSize(Vector3 size) {
    this->extents = size / 2;
    if ((this->extents < 0).Any()) {
        Error("An oriented bounding box cannot have negative extents.");
        SetNull();
        return;
    }
    CheckParams();
}
void Starsurge::OBB::SetOrientation(Vector3 axis) {
    Vector3 axis2, axis3;
    if (axis.x != 0 && axis.z == 0) { axis2 = Vector3(0,0,1); }
    else if (axis.x == 0 && axis.z == 0) { axis2 = Vector3(1,0,0); }
    else { axis2 = Vector3(1,0,-axis.x/axis.z); }
    axis3 = Vector3::CrossProduct(axis, axis2);
    SetOrientation(axis, axis2, axis3);
}
void Starsurge::OBB::SetOrientation(Vector3 axis1, Vector3 axis2) {
    SetOrientation(axis1, axis2, Vector3::CrossProduct(axis1, axis2));
}
void Starsurge::OBB::SetOrientation(Vector3 axis1, Vector3 axis2, Vector3 axis3) {
    if (axis1.SquaredMagnitude() == 0 || axis2.SquaredMagnitude() == 0 || axis3.SquaredMagnitude() == 0) {
        Error("Cannot use a zero vector in an orthonormal basis.");
        SetNull();
        return;
    }

    this->u = axis1;
    this->v = axis2;
    this->w = axis3;
    this->u.Normalize();
    this->v.Normalize();
    this->w.Normalize();

    float eps = 0.00001;
    if (Abs(Vector3::Dot(this->u, this->v)) >= eps || Abs(Vector3::Dot(this->u, this->w)) >= eps || Abs(Vector3::Dot(this->v, this->w)) >= eps) {
        std::vector<Vector3> set = { this->u, this->v, this->w };
        set = Vector3::GramSchmidt(set);
        if (set.size() == 0) {
            Error("Couldn't form an orthonormal basis for OBB.");
            SetNull();
            return;
        }
        this->u = set[0];
        this->v = set[1];
        this->w = set[2];
    }

    CheckParams();
}
void Starsurge::OBB::SetOrientation(Matrix3 m) {
    SetOrientation(m.GetColumn(0), m.GetColumn(1), m.GetColumn(2));
}

bool Starsurge::OBB::IsNull() const {
    return this->isNull;
}
bool Starsurge::OBB::Contains(const Vector3 pt) const {
    if (IsNull()) {
        return false;
    }
    Matrix3 mat = GetOrientation();
    Vector3 localPt = mat.Transpose()*(pt-this->Center);
    return AABB(-this->extents, this->extents).Contains(localPt);
}
Starsurge::Vector3 Starsurge::OBB::ClosestPoint(const Vector3 point) const {
    if (IsNull() || Contains(point)) {
        return point;
    }
    Matrix3 mat = GetOrientation();
    Vector3 localPt = mat.Transpose()*(point-this->Center);
    Vector3 closest = AABB(-this->extents, this->extents).ClosestPoint(localPt);
    return mat*closest+this->Center;
}
float Starsurge::OBB::Distance(const Vector3 point) const {
    if (IsNull()) {
        return -1;
    }
    Vector3 closest = ClosestPoint(point);
    return (closest - point).Magnitude();
}

Starsurge::Vector3 Starsurge::OBB::GetCorner(BoxCorner corner) const {
    Vector3 c = this->Center;
    float x = this->extents.x, y = this->extents.y, z = this->extents.z;
    switch (corner) {
        case FAR_LEFT_BOTTOM: return c-x*this->u-y*this->v-z*this->w;
        case FAR_LEFT_TOP: return c-x*this->u+y*this->v-z*this->w;
        case FAR_RIGHT_TOP: return c+x*this->u+y*this->v-z*this->w;
        case FAR_RIGHT_BOTTOM: return c+x*this->u-y*this->v-z*this->w;
        case NEAR_RIGHT_BOTTOM: return c+x*this->u-y*this->v+z*this->w;
        case NEAR_LEFT_BOTTOM: return c-x*this->u-y*this->v+z*this->w;
        case NEAR_LEFT_TOP: return c-x*this->u+y*this->v+z*this->w;
        case NEAR_RIGHT_TOP: return c+x*this->u+y*this->v+z*this->w;
        default:
            Error("Invalid corner in OBB::GetCorner(). Options: FAR_LEFT_BOTTOM, FAR_LEFT_TOP, FAR_RIGHT_TOP, FAR_RIGHT_BOTTOM, NEAR_RIGHT_BOTTOM, NEAR_LEFT_BOTTOM, NEAR_LEFT_TOP, NEAR_RIGHT_TOP");
            return Vector3(0,0,0);
    }
}
std::vector<Starsurge::Vector3> Starsurge::OBB::GetAllCorners() const {
    std::vector<Vector3> ret;
    ret.push_back(GetCorner(FAR_LEFT_BOTTOM));
    ret.push_back(GetCorner(FAR_LEFT_TOP));
    ret.push_back(GetCorner(FAR_RIGHT_TOP));
    ret.push_back(GetCorner(FAR_RIGHT_BOTTOM));
    ret.push_back(GetCorner(NEAR_RIGHT_BOTTOM));
    ret.push_back(GetCorner(NEAR_LEFT_BOTTOM));
    ret.push_back(GetCorner(NEAR_LEFT_TOP));
    ret.push_back(GetCorner(NEAR_RIGHT_TOP));
    return ret;
}
Starsurge::Line Starsurge::OBB::GetEdge(BoxCorner corner1, BoxCorner corner2) const {
    std::map<BoxCorner, std::vector<BoxCorner>> validEdges;
    validEdges[FAR_LEFT_BOTTOM] = { FAR_LEFT_TOP, FAR_RIGHT_BOTTOM, NEAR_LEFT_BOTTOM };
    validEdges[FAR_LEFT_TOP] = { FAR_LEFT_BOTTOM, FAR_RIGHT_TOP, NEAR_LEFT_TOP };
    validEdges[FAR_RIGHT_TOP] = { FAR_LEFT_TOP, FAR_RIGHT_BOTTOM, NEAR_RIGHT_TOP };
    validEdges[FAR_RIGHT_BOTTOM] = { FAR_LEFT_BOTTOM, FAR_RIGHT_TOP, NEAR_RIGHT_BOTTOM };
    validEdges[NEAR_RIGHT_BOTTOM] = { NEAR_LEFT_BOTTOM, NEAR_RIGHT_TOP, FAR_RIGHT_BOTTOM };
    validEdges[NEAR_LEFT_BOTTOM] = { NEAR_RIGHT_BOTTOM, NEAR_LEFT_TOP, FAR_LEFT_BOTTOM };
    validEdges[NEAR_LEFT_TOP] = { NEAR_RIGHT_TOP, NEAR_LEFT_BOTTOM, FAR_LEFT_TOP };
    validEdges[NEAR_RIGHT_TOP] = { NEAR_LEFT_TOP, NEAR_RIGHT_BOTTOM, FAR_RIGHT_TOP };

    if (ElemOf<BoxCorner>(validEdges[corner1], corner2)) {
        return Line(GetCorner(corner1), GetCorner(corner2));
    }
    else {
        Error("Not a valid edge. Corners are not connected.");
        return Line(Vector3(0,0,0),Vector3(0,0,0));
    }
}
std::vector<Starsurge::Line> Starsurge::OBB::GetAllEdges() const {
    std::map<BoxCorner, std::vector<BoxCorner>> validEdges;
    validEdges[FAR_LEFT_BOTTOM] = { FAR_LEFT_TOP, FAR_RIGHT_BOTTOM, NEAR_LEFT_BOTTOM };
    validEdges[FAR_LEFT_TOP] = { FAR_LEFT_BOTTOM, FAR_RIGHT_TOP, NEAR_LEFT_TOP };
    validEdges[FAR_RIGHT_TOP] = { FAR_LEFT_TOP, FAR_RIGHT_BOTTOM, NEAR_RIGHT_TOP };
    validEdges[FAR_RIGHT_BOTTOM] = { FAR_LEFT_BOTTOM, FAR_RIGHT_TOP, NEAR_RIGHT_BOTTOM };
    validEdges[NEAR_RIGHT_BOTTOM] = { NEAR_LEFT_BOTTOM, NEAR_RIGHT_TOP, FAR_RIGHT_BOTTOM };
    validEdges[NEAR_LEFT_BOTTOM] = { NEAR_RIGHT_BOTTOM, NEAR_LEFT_TOP, FAR_LEFT_BOTTOM };
    validEdges[NEAR_LEFT_TOP] = { NEAR_RIGHT_TOP, NEAR_LEFT_BOTTOM, FAR_LEFT_TOP };
    validEdges[NEAR_RIGHT_TOP] = { NEAR_LEFT_TOP, NEAR_RIGHT_BOTTOM, FAR_RIGHT_TOP };

    std::vector<Line> edges;
    for (auto const& x : validEdges) {
        for (unsigned int i = 0; i < x.second.size(); ++i) {
            Line line(GetCorner(x.first), GetCorner(x.second[i]));
            if (!ElemOf<Line>(edges, line)) {
                edges.push_back(line);
            }
        }
    }
    return edges;
}
Starsurge::Quad Starsurge::OBB::GetFace(BoxFace face) const {
    switch (face) {
        case LEFT_FACE: {
            Quad quad(GetCorner(FAR_LEFT_BOTTOM), GetCorner(FAR_LEFT_TOP), GetCorner(NEAR_LEFT_TOP), GetCorner(NEAR_LEFT_BOTTOM));
            quad.FlipNormal();
            return quad;
        }
        case TOP_FACE: {
            Quad quad(GetCorner(FAR_LEFT_TOP), GetCorner(FAR_RIGHT_TOP), GetCorner(NEAR_RIGHT_TOP), GetCorner(NEAR_LEFT_TOP));
            return quad;
        }
        case RIGHT_FACE: {
            Quad quad(GetCorner(FAR_RIGHT_BOTTOM), GetCorner(FAR_RIGHT_TOP), GetCorner(NEAR_RIGHT_TOP), GetCorner(NEAR_RIGHT_BOTTOM));
            return quad;
        }
        case BOTTOM_FACE: {
            Quad quad(GetCorner(FAR_LEFT_BOTTOM), GetCorner(FAR_RIGHT_BOTTOM), GetCorner(NEAR_RIGHT_BOTTOM), GetCorner(NEAR_LEFT_BOTTOM));
            quad.FlipNormal();
            return quad;
        }
        case NEAR_FACE: {
            Quad quad(GetCorner(NEAR_LEFT_BOTTOM), GetCorner(NEAR_RIGHT_BOTTOM), GetCorner(NEAR_RIGHT_TOP), GetCorner(NEAR_LEFT_TOP));
            quad.FlipNormal();
            return quad;
        }
        case FAR_FACE: {
            Quad quad(GetCorner(FAR_LEFT_BOTTOM), GetCorner(FAR_RIGHT_BOTTOM), GetCorner(FAR_RIGHT_TOP), GetCorner(FAR_LEFT_TOP));
            return quad;
        }
        default:
            Error("Invalid face in OBB::GetFace(). Options: LEFT_FACE, TOP_FACE, RIGHT_FACE, BOTTOM_FACE, NEAR_FACE, FAR_FACE");
            return Quad();
    }
}
std::vector<Starsurge::Quad> Starsurge::OBB::GetAllFaces() const {
    std::vector<Quad> faces;
    faces.push_back(GetFace(LEFT_FACE));
    faces.push_back(GetFace(TOP_FACE));
    faces.push_back(GetFace(RIGHT_FACE));
    faces.push_back(GetFace(BOTTOM_FACE));
    faces.push_back(GetFace(NEAR_FACE));
    faces.push_back(GetFace(FAR_FACE));
    return faces;
}

Starsurge::Vector3 Starsurge::OBB::GetSize() const {
    return 2*this->extents;
}
Starsurge::Vector3 Starsurge::OBB::GetExtents() const {
    return this->extents;
}
float Starsurge::OBB::GetVolume() const {
    Vector3 size = GetSize();
    return size.x*size.y*size.z;
}
Starsurge::Vector3 Starsurge::OBB::GetAxis(unsigned int i) const {
    switch (i) {
        case 0: return this->u;
        case 1: return this->v;
        case 2: return this->w;
        default:
            Error("Invalid axis in OBB::GetAxis(). Options: 0, 1, 2");
            return Vector3(0,0,0);
    }
}
Starsurge::Matrix3 Starsurge::OBB::GetOrientation() const {
    Matrix3 ret = { this->u, this->v, this->w };
    return ret;
}
Starsurge::AABB Starsurge::OBB::FitAABB() const {
    if (IsNull()) {
        return AABB::Null();
    }

    std::vector<Vector3> corners = GetAllCorners();
    Vector3 min = corners[0], max = corners[0];
    for (unsigned int i = 1; i < corners.size(); ++i) {
        min = Vector3::Min(min, corners[i]);
        max = Vector3::Max(max, corners[i]);
    }
    return AABB(min, max);
}

void Starsurge::OBB::SetNull() {
    this->isNull = true;
}
void Starsurge::OBB::Expand(float amount) {
    Expand(Vector3(amount, amount, amount));
}
void Starsurge::OBB::Expand(Vector3 amount) {
    if (IsNull()) {
        return;
    }
    this->extents += amount;
    CheckParams();
}

std::string Starsurge::OBB::ToString(unsigned int ndigits) {
    if (IsNull()) {
        return "OBB {Null}";
    }
    std::string ret = "OBB";
    ret += " {C = "+this->Center.ToString(ndigits);
    ret += ", u = "+this->u.ToString(ndigits);
    ret += ", v = "+this->v.ToString(ndigits);
    ret += ", w = "+this->w.ToString(ndigits);
    ret += ", e = "+this->extents.ToString(ndigits);
    ret += "}";
    return ret;
}

Starsurge::OBB Starsurge::OBB::Null() {
    OBB ret;
    ret.SetNull();
    return ret;
}

void Starsurge::OBB::Transform(const Matrix<3,3> matrix) {
    if (IsNull()) {
        return;
    }

    SetOrientation(matrix*this->u, matrix*this->v, matrix*this->w);
    this->Center = matrix*this->Center;
}
void Starsurge::OBB::Transform(const Matrix<4,4> matrix) {
    if (IsNull()) {
        return;
    }

    Vector3 v1 = (matrix*Vector4(this->u, 0)).Resize<3>();
    Vector3 v2 = (matrix*Vector4(this->v, 0)).Resize<3>();
    Vector3 v3 = (matrix*Vector4(this->w, 0)).Resize<3>();
    SetOrientation(v1, v2, v3);
    this->Center = (matrix*Vector4(this->Center, 1)).Resize<3>();
}

Starsurge::Mesh Starsurge::OBB::CreateMesh() const {
    if (IsNull()) {
        return Mesh(std::vector<Vertex>(), std::vector<unsigned int>());
    }

    std::vector<Vector3> Positions = {
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3( 0.5f,  0.5f, -0.5f),
        Vector3( 0.5f, -0.5f, -0.5f),
        Vector3( 0.5f,  0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(-0.5f,  0.5f, -0.5f),

        Vector3(-0.5f, -0.5f,  0.5f),
        Vector3( 0.5f, -0.5f,  0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3(-0.5f,  0.5f,  0.5f),
        Vector3(-0.5f, -0.5f,  0.5f),

        Vector3(-0.5f,  0.5f,  0.5f),
        Vector3(-0.5f,  0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3(-0.5f, -0.5f,  0.5f),
        Vector3(-0.5f,  0.5f,  0.5f),

        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3( 0.5f, -0.5f, -0.5f),
        Vector3( 0.5f,  0.5f, -0.5f),
        Vector3( 0.5f, -0.5f, -0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3( 0.5f, -0.5f,  0.5f),

        Vector3(-0.5f, -0.5f, -0.5f),
        Vector3( 0.5f, -0.5f, -0.5f),
        Vector3( 0.5f, -0.5f,  0.5f),
        Vector3( 0.5f, -0.5f,  0.5f),
        Vector3(-0.5f, -0.5f,  0.5f),
        Vector3(-0.5f, -0.5f, -0.5f),

        Vector3(-0.5f,  0.5f, -0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3( 0.5f,  0.5f, -0.5f),
        Vector3( 0.5f,  0.5f,  0.5f),
        Vector3(-0.5f,  0.5f, -0.5f),
        Vector3(-0.5f,  0.5f,  0.5f)
    };
    std::vector<Vector2> UV = {
        Vector2(0.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 0.0f),
        Vector2(0.0f, 1.0f),

        Vector2(0.0f, 0.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 0.0f),

        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 0.0f),
        Vector2(1.0f, 0.0f),

        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 1.0f),
        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 0.0f),

        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 0.0f),
        Vector2(0.0f, 1.0f),

        Vector2(0.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(1.0f, 1.0f),
        Vector2(1.0f, 0.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, 0.0f)
    };
    std::vector<Vector3> Normals = {
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),
        Vector3( 0.0f,  0.0f, -1.0f),

        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),
        Vector3( 0.0f,  0.0f,  1.0f),

        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),
        Vector3(-1.0f,  0.0f,  0.0f),

        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),
        Vector3( 1.0f,  0.0f,  0.0f),

        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),
        Vector3( 0.0f, -1.0f,  0.0f),

        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f),
        Vector3( 0.0f,  1.0f,  0.0f)
    };

    std::vector<Vertex> vertices;
    Vector3 size = GetSize();
    Matrix3 transform = GetOrientation();
    for (size_t i = 0; i < Positions.size(); ++i) {
        Vertex v;
        v.Position = transform*Vector3::EntrywiseProduct(Positions[i], size); // Mesh centered at origin.
        v.Normal = transform*Normals[i];
        v.UV = UV[i];
        v.Color = Colors::WHITE;
        vertices.push_back(v);
    }
    std::vector<unsigned int> indices;
    return Mesh(vertices, indices);
}

void Starsurge::OBB::CheckParams() {
    float eps = 0.00001;
    if ((this->extents < 0).Any() || this->u.SquaredMagnitude() == 0 || this->v.SquaredMagnitude() == 0 || this->w.SquaredMagnitude() == 0 ||
        Abs(Vector3::Dot(this->u, this->v)) >= eps || Abs(Vector3::Dot(this->u, this->w)) >= eps || Abs(Vector3::Dot(this->v, this->w)) >= eps) {
        this->isNull = true;
    }
    else {
        this->isNull = false;
    }
}
