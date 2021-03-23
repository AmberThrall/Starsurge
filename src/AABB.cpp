#include "../include/AABB.h"
#include "../include/Logging.h"

Starsurge::AABB::AABB() {
    this->isNull = true;
    this->minimum = Vector3(0,0,0);
    this->maximum = Vector3(0,0,0);
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
void Starsurge::AABB::SetBounds(float mx, float my, float mz, float Mx, float My, float Mz) {
    this->minimum = Vector3(mx, my, mz);
    this->maximum = Vector3(Mx, My, Mz);
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
        this->isNull = true;
        this->minimum = Vector3(0,0,0);
        this->maximum = Vector3(0,0,0);
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

bool Starsurge::AABB::Contains(const Sphere sphere) const {
    if (IsNull()) {
        return false;
    }

    // Get the 6 maximum/minimum points on the sphere.
    Vector3 up = sphere.position + Vector3::Up()*sphere.radius;
    Vector3 down = sphere.position - Vector3::Up()*sphere.radius;
    Vector3 right = sphere.position + Vector3::Right()*sphere.radius;
    Vector3 left = sphere.position - Vector3::Right()*sphere.radius;
    Vector3 forward = sphere.position + Vector3::Forward()*sphere.radius;
    Vector3 backward = sphere.position - Vector3::Forward()*sphere.radius;

    // Test each one.
    return (Contains(up) && Contains(down) && Contains(right) && Contains(left) &&
            Contains(forward) && Contains(backward));
}

Starsurge::Vector3 Starsurge::AABB::GetCenter() const {
    return (this->maximum + this->minimum) / 2;
}

Starsurge::Vector3 Starsurge::AABB::GetSize() const {
    return this->maximum - this->minimum;
}

float Starsurge::AABB::GetVolume() const {
    Vector3 size = GetSize();
    return size.x*size.y*size.z;
}

Starsurge::Vector3 Starsurge::AABB::GetCorner(AABBCorner corner) const {
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

Starsurge::Vector3 Starsurge::AABB::GetMinimum() const {
    return this->minimum;
}
Starsurge::Vector3 Starsurge::AABB::GetMaximum() const {
    return this->maximum;
}
bool Starsurge::AABB::IsNull() const {
    return this->isNull;
}
Starsurge::Vector3 Starsurge::AABB::ClosestPoint(const Vector3 point) const {
    if (Contains(point)) {
        return point;
    }
    return Vector3::Clamp(point, this->minimum, this->maximum);
}

std::string Starsurge::AABB::ToString() {
    if (IsNull()) {
        return "AABB {Null}";
    }
    return "AABB {min = "+this->minimum.ToString()+", max = "+this->maximum.ToString() + "}";
}

Starsurge::AABB Starsurge::AABB::Null() {
    AABB ret;
    return ret;
}
Starsurge::AABB Starsurge::AABB::Union(const AABB box, const Vector3 pt) {
    if (box.IsNull()) {
        return AABB(pt, pt);
    }
    return AABB(Vector3::Min(box.GetMinimum(), pt), Vector3::Max(box.GetMaximum(), pt));
}

Starsurge::AABB Starsurge::AABB::Union(const AABB box, const std::vector<Vector3> pts) {
    AABB ret(box);
    for (unsigned int i = 0; i < pts.size(); ++i) {
        ret = AABB::Union(ret, pts[i]);
    }
    return ret;
}

Starsurge::AABB Starsurge::AABB::Union(const AABB boxA, const AABB boxB) {
    if (boxA.IsNull()) {
        return boxB;
    }
    if (boxB.IsNull()) {
        return boxA;
    }
    return AABB(Vector3::Min(boxA.GetMinimum(), boxB.GetMinimum()), Vector3::Max(boxA.GetMaximum(), boxB.GetMaximum()));
}

Starsurge::AABB Starsurge::AABB::Transform(const Matrix3 matrix) {
    if (IsNull()) {
        return AABB::Null();
    }

    AABB ret;
    AABBCorner corners[] = { FAR_LEFT_BOTTOM, NEAR_LEFT_BOTTOM, NEAR_LEFT_TOP, FAR_LEFT_TOP, FAR_RIGHT_TOP, NEAR_RIGHT_TOP,
        NEAR_RIGHT_BOTTOM, FAR_RIGHT_BOTTOM };
    for (unsigned int i = 0; i < 8; ++i) {
        ret = AABB::Union(ret, matrix * GetCorner(corners[i]));
    }
    return ret;
}

Starsurge::AABB Starsurge::AABB::Transform(const Matrix4 matrix) {
    if (IsNull()) {
        return AABB::Null();
    }

    AABB ret;
    AABBCorner corners[] = { FAR_LEFT_BOTTOM, NEAR_LEFT_BOTTOM, NEAR_LEFT_TOP, FAR_LEFT_TOP, FAR_RIGHT_TOP, NEAR_RIGHT_TOP,
        NEAR_RIGHT_BOTTOM, FAR_RIGHT_BOTTOM };
    for (unsigned int i = 0; i < 8; ++i) {
        Vector4 corner = Vector4(GetCorner(corners[i]), 1);
        ret = AABB::Union(ret, (matrix * corner).SubVector<3>());
    }
    return ret;
}

void Starsurge::AABB::CheckBounds() {
    if ((this->minimum > this->maximum).Any()) {
        Error("For AABB, the minimum value must be less than or equal to the maximum value.");
        this->isNull = true;
        this->minimum = this->maximum;
    }
}
