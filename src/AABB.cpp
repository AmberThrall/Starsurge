#include <map>
#include "../include/AABB.h"
#include "../include/Logging.h"

Starsurge::AABB::AABB() {
    SetNull();
}
Starsurge::AABB::AABB(const AABB& copy) {
    SetBounds(copy);
}
Starsurge::AABB::AABB(float mx, float my, float mz, float Mx, float My, float Mz) {
    SetBounds(mx, my, mz, Mx, My, Mz);
}
Starsurge::AABB::AABB(Vector3 min, Vector3 max) {
    SetBounds(min, max);
}
Starsurge::AABB::AABB(std::vector<Vector3> points) {
    SetBounds(points);
}

void Starsurge::AABB::SetBounds(const AABB& copy) {
    this->minimum = copy.minimum;
    this->maximum = copy.maximum;
    this->isNull = copy.IsNull();
    CheckBounds();
}
void Starsurge::AABB::SetBounds(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax) {
    this->minimum = Vector3(xmin, ymin, zmin);
    this->maximum = Vector3(xmax, ymax, zmax);
    this->isNull = false;
    CheckBounds();
}
void Starsurge::AABB::SetBounds(Vector3 min, Vector3 max) {
    this->minimum = min;
    this->maximum = max;
    this->isNull = false;
    CheckBounds();
}
void Starsurge::AABB::SetBounds(std::vector<Vector3> points) {
    if (points.size() == 0) {
        SetNull();
        return;
    }

    this->minimum = points[0];
    this->maximum = points[0];
    this->isNull = false;
    for (unsigned int i = 0; i < points.size(); ++i) {
        Vector3 p = points[i];
        this->minimum = Vector3::Min(this->minimum, p);
        this->maximum = Vector3::Max(this->maximum, p);
    }
    CheckBounds();
}

bool Starsurge::AABB::Contains(const Vector3 pt) const {
    if (IsNull()) {
        return false;
    }

    return (pt.x >= this->minimum.x && pt.x <= this->maximum.x &&
            pt.y >= this->minimum.y && pt.y <= this->maximum.y &&
            pt.z >= this->minimum.z && pt.z <= this->maximum.z);
}

bool Starsurge::AABB::Contains(const AABB box) const {
    if (IsNull() || box.IsNull()) {
        return false;
    }

    Vector3 min = box.GetMinimum();
    Vector3 max = box.GetMaximum();
    return (min.x >= this->minimum.x && max.x <= this->maximum.x &&
            min.y >= this->minimum.y && max.y <= this->maximum.y &&
            min.z >= this->minimum.z && max.z <= this->maximum.z);
}

Starsurge::Vector3 Starsurge::AABB::ClosestPoint(const Vector3 point) const {
    if (IsNull() || Contains(point)) {
        return point;
    }
    return Vector3::Clamp(point, this->minimum, this->maximum);
}
float Starsurge::AABB::Distance(const Vector3 point) const {
    if (IsNull()) {
        return -1;
    }
    Vector3 closest = ClosestPoint(point);
    return (closest - point).Magnitude();
}

Starsurge::Vector3 Starsurge::AABB::GetCenter() const {
    return (this->maximum + this->minimum) / 2;
}
Starsurge::Vector3 Starsurge::AABB::GetSize() const {
    return this->maximum - this->minimum;
}
Starsurge::Vector3 Starsurge::AABB::GetExtents() const {
    return this->maximum - GetCenter();
}

float Starsurge::AABB::GetVolume() const {
    Vector3 size = GetSize();
    return size.x*size.y*size.z;
}

Starsurge::Vector3 Starsurge::AABB::GetCorner(BoxCorner corner) const {
    float mx = this->minimum.x, my = this->minimum.y, mz = this->minimum.z;
    float Mx = this->maximum.x, My = this->maximum.y, Mz = this->maximum.z;

    switch (corner) {
        case FAR_LEFT_BOTTOM: return this->minimum;
        case FAR_LEFT_TOP: return Vector3(mx, My, mz);
        case FAR_RIGHT_TOP: return Vector3(Mx, My, mz);
        case FAR_RIGHT_BOTTOM: return Vector3(Mx, my, mz);
        case NEAR_RIGHT_BOTTOM: return Vector3(Mx, my, Mz);
        case NEAR_LEFT_BOTTOM: return Vector3(mx, my, Mz);
        case NEAR_LEFT_TOP: return Vector3(mx, My, Mz);
        case NEAR_RIGHT_TOP: return this->maximum;
        default:
            Error("Invalid corner in AABB::GetCorner(). Options: FAR_LEFT_BOTTOM, FAR_LEFT_TOP, FAR_RIGHT_TOP, FAR_RIGHT_BOTTOM, NEAR_RIGHT_BOTTOM, NEAR_LEFT_BOTTOM, NEAR_LEFT_TOP, NEAR_RIGHT_TOP");
            return Vector3(0,0,0);
    }
}
std::vector<Starsurge::Vector3> Starsurge::AABB::GetAllCorners() const {
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
Starsurge::Line Starsurge::AABB::GetEdge(BoxCorner corner1, BoxCorner corner2) const {
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
std::vector<Starsurge::Line> Starsurge::AABB::GetAllEdges() const {
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
Starsurge::Quad Starsurge::AABB::GetFace(BoxFace face) const {
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
            Error("Invalid face in AABB::GetFace(). Options: LEFT_FACE, TOP_FACE, RIGHT_FACE, BOTTOM_FACE, NEAR_FACE, FAR_FACE");
            return Quad();
    }
}
std::vector<Starsurge::Quad> Starsurge::AABB::GetAllFaces() const {
    std::vector<Quad> faces;
    faces.push_back(GetFace(LEFT_FACE));
    faces.push_back(GetFace(TOP_FACE));
    faces.push_back(GetFace(RIGHT_FACE));
    faces.push_back(GetFace(BOTTOM_FACE));
    faces.push_back(GetFace(NEAR_FACE));
    faces.push_back(GetFace(FAR_FACE));
    return faces;
}

Starsurge::Vector3 Starsurge::AABB::GetMinimum() const {
    return this->minimum;
}
Starsurge::Vector3 Starsurge::AABB::GetMaximum() const {
    return this->maximum;
}
bool Starsurge::AABB::IsNull() const {
    return this->isNull;
}

void Starsurge::AABB::Expand(float amount) {
    Expand(Vector3(amount,amount,amount));
}
void Starsurge::AABB::Expand(Vector3 amount) {
    this->minimum -= amount;
    this->maximum += amount;
    CheckBounds(false);
}
void Starsurge::AABB::SetNull() {
    this->isNull = true;
}

std::string Starsurge::AABB::ToString(unsigned int ndigits) {
    if (IsNull()) {
        return "AABB {Null}";
    }
    return "AABB {min = "+this->minimum.ToString(ndigits)+", max = "+this->maximum.ToString(ndigits) + "}";
}

Starsurge::AABB Starsurge::AABB::Null() {
    AABB ret;
    ret.SetNull();
    return ret;
}
void Starsurge::AABB::Union(const Vector3 pt) {
    if (IsNull()) {
        this->minimum = pt;
        this->maximum = pt;
        this->isNull = false;
        return;
    }
    this->minimum = Vector3::Min(this->minimum, pt);
    this->maximum = Vector3::Max(this->maximum, pt);
}
void Starsurge::AABB::Union(const std::vector<Vector3> pts) {
    for (unsigned int i = 0; i < pts.size(); ++i) {
        Union(pts[i]);
    }
}
void Starsurge::AABB::Union(const AABB boxB) {
    if (IsNull()) {
        SetBounds(boxB);
        return;
    }
    if (boxB.IsNull()) {
        return;
    }
    this->minimum = Vector3::Min(this->minimum, boxB.GetMinimum());
    this->maximum = Vector3::Max(this->maximum, boxB.GetMaximum());
}

void Starsurge::AABB::Subtract(const Vector3 pt) {
    if (IsNull()) {
        return;
    }

    if (this->minimum == this->maximum && this->minimum == pt) {
        SetNull();
        return;
    }

    if (!Contains(pt)) {
        return;
    }

    // Get the minimal difference in x,y,z in the 6 faces.
    float diff1 = pt.x-this->minimum.x;
    float diff2 = pt.y-this->minimum.y;
    float diff3 = pt.z-this->minimum.z;
    float diff4 = this->maximum.x-pt.x;
    float diff5 = this->maximum.y-pt.y;
    float diff6 = this->maximum.z-pt.z;
    float min = Min(Min(Min(diff1, diff2), Min(diff3, diff4)), Min(diff5,diff6));

    // Move whatever face has the least distance to go. This should maximize volume.
    float eps = 0.00001; // Add a bit of wiggle room so the point doesn't actually lie in the AABB anymore.
    if (min == diff1) { this->minimum.x = pt.x+eps; }
    else if (min == diff2) { this->minimum.y = pt.y+eps; }
    else if (min == diff3) { this->minimum.z = pt.z+eps; }
    else if (min == diff4) { this->maximum.x = pt.x-eps; }
    else if (min == diff5) { this->maximum.y = pt.y-eps; }
    else { this->maximum.z = pt.z-eps; }
    CheckBounds(false);
}
void Starsurge::AABB::Subtract(const std::vector<Vector3> pts) {
    for (unsigned int i = 0; i < pts.size(); ++i) {
        if (IsNull())
            break;
        Subtract(pts[i]);
    }
}
void Starsurge::AABB::Subtract(const AABB boxB) {
    if (IsNull() || boxB.IsNull()) {
        return;
    }

    AABB intersection(*this);
    intersection.Intersection(boxB);
    if (intersection.IsNull()) {
        if (Contains(intersection.GetMinimum())) { Subtract(intersection.GetMinimum()); }
        else { Subtract(intersection.GetMaximum()); }
    }
}

void Starsurge::AABB::Intersection(const AABB boxB) {
    if (IsNull() || boxB.IsNull()) {
        SetNull();
        return;
    }

    if (this->minimum.x <= boxB.GetMaximum().x && this->maximum.x >= boxB.GetMinimum().x &&
        this->minimum.y <= boxB.GetMaximum().y && this->maximum.y >= boxB.GetMinimum().y &&
        this->minimum.z <= boxB.GetMaximum().z && this->maximum.z >= boxB.GetMinimum().z) {
        this->minimum = Vector3::Max(this->minimum, boxB.GetMinimum());
        this->maximum = Vector3::Min(this->maximum, boxB.GetMaximum());
    }
    else {
        SetNull();
    }
}


Starsurge::OBB Starsurge::AABB::AsOBB() const {
    if (IsNull()) {
        return OBB::Null();
    }
    return OBB(GetSize(), GetCenter(), Vector3(1,0,0), Vector3(0,1,0), Vector3(0,0,1));
}

void Starsurge::AABB::Transform(const Matrix3 matrix) {
    if (IsNull()) {
        return;
    }

    std::vector<Vector3> corners = GetAllCorners();
    SetNull();
    for (unsigned int i = 0; i < 8; ++i) {
        Union(matrix * corners[i]);
    }
}

void Starsurge::AABB::Transform(const Matrix4 matrix) {
    if (IsNull()) {
        return;
    }

    std::vector<Vector3> corners = GetAllCorners();
    SetNull();
    for (unsigned int i = 0; i < 8; ++i) {
        Union((matrix * Vector4(corners[i], 1)).Resize<3>());
    }
}

Starsurge::Mesh Starsurge::AABB::CreateMesh() const {
    return AsOBB().CreateMesh();
}

void Starsurge::AABB::CheckBounds(bool raiseError) {
    if ((this->minimum > this->maximum).Any()) {
        if (raiseError) {
            Error("For AABB, the minimum value must be less than or equal to the maximum value.");
        }
        SetNull();
    }
}
