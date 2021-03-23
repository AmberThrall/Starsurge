#include "../include/Rect.h"

Starsurge::Rect::Rect() {
    this->isNull = true;
    this->min = Vector2(0,0);
    this->max = Vector2(0,0);
}
Starsurge::Rect::Rect(const Rect& copy) {
    SetBounds(copy);
}
Starsurge::Rect::Rect(Vector2 t_min, Vector2 t_max) {
    SetBounds(t_min, t_max);
}
Starsurge::Rect::Rect(float xmin, float ymin, float xmax, float ymax) {
    SetBounds(xmin, ymin, xmax, ymax);
}
Starsurge::Rect::Rect(std::vector<Vector2> points) {
    SetBounds(points);
}

void Starsurge::Rect::SetBounds(const Rect& copy) {
    this->min = copy.GetMinimum();
    this->max = copy.GetMaximum();
    this->isNull = copy.IsNull();
    CheckBounds();
}
void Starsurge::Rect::SetBounds(Vector2 t_min, Vector2 t_max) {
    this->min = t_min;
    this->max = t_max;
    this->isNull = false;
    CheckBounds();
}
void Starsurge::Rect::SetBounds(float xmin, float ymin, float xmax, float ymax) {
    this->min = Vector2(xmin, ymin);
    this->max = Vector2(xmax, ymax);
    this->isNull = false;
    CheckBounds();
}
void Starsurge::Rect::SetBounds(std::vector<Vector2> points) {
    if (points.size() == 0) {
        this->isNull = true;
        this->min = Vector2(0,0);
        this->max = Vector2(0,0);
        return;
    }

    this->min = points[0];
    this->max = points[0];
    for (unsigned int i = 1; i < points.size(); ++i) {
        this->min = Vector2::Min(this->min, points[i]);
        this->max = Vector2::Max(this->max, points[i]);
    }
    CheckBounds();
}

std::string Starsurge::Rect::ToString() const {
    if (IsNull()) {
        return "Rect {Null}";
    }
    return "Rect {min = "+this->min.ToString()+", max = "+this->max.ToString() + "}";
}

bool Starsurge::Rect::IsNull() const {
    return this->isNull;
}
Starsurge::Vector2 Starsurge::Rect::GetMinimum() const {
    return this->min;
}
Starsurge::Vector2 Starsurge::Rect::GetMaximum() const {
    return this->max;
}
Starsurge::Vector2 Starsurge::Rect::GetCenter() const {
    return (this->max + this->min) / 2;
}
Starsurge::Vector2 Starsurge::Rect::GetSize() const {
    return this->max - this->min;
}
float Starsurge::Rect::GetArea() const {
    Vector2 size = GetSize();
    return size.x*size.y;
}

Starsurge::Vector2 Starsurge::Rect::GetCorner(RectCorner corner) const {
    switch (corner) {
        case TOP_LEFT: return this->min;
        case TOP_RIGHT: return Vector2(this->max.x, this->min.y);
        case BOTTOM_LEFT: return Vector2(this->min.x, this->max.y);
        case BOTTOM_RIGHT: return this->max;
        default:
            Error("Invalid corner in Rect::GetCorner(). Options: TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT");
            return Vector2(0,0);
    }
}
std::vector<Starsurge::Vector2> Starsurge::Rect::GetAllCorners() const {
    std::vector<Vector2> ret;
    ret.push_back(GetCorner(TOP_LEFT));
    ret.push_back(GetCorner(TOP_RIGHT));
    ret.push_back(GetCorner(BOTTOM_LEFT));
    ret.push_back(GetCorner(BOTTOM_RIGHT));
    return ret;
}

bool Starsurge::Rect::Contains(Vector2 p) const {
    if (IsNull()) {
        return false;
    }

    return (p.x >= this->min.x && p.x <= this->max.x && p.y >= this->min.y && p.y <= this->max.y);
}
bool Starsurge::Rect::Contains(Rect r) const {
    if (IsNull() || r.IsNull()) {
        return false;
    }

    Vector2 min = r.GetMinimum();
    Vector2 max = r.GetMaximum();
    return (min.x >= this->min.x && max.x <= this->max.x &&
            min.y >= this->min.y && max.y <= this->max.y);
}

Starsurge::Vector2 Starsurge::Rect::ClosestPoint(const Vector2 point) const {
    if (Contains(point)) {
        return point;
    }
    return Vector2::Clamp(point, this->min, this->max);
}

Starsurge::Rect Starsurge::Rect::Expand(float amount) {
    return Expand(Vector2(amount, amount));
}
Starsurge::Rect Starsurge::Rect::Expand(Vector2 amount) {
    return Rect(this->min - amount, this->max + amount);
}

Starsurge::Rect Starsurge::Rect::Null() {
    Rect ret;
    return ret;
}
Starsurge::Rect Starsurge::Rect::Union(const Rect r, const Vector2 pt) {
    if (r.IsNull()) {
        return Rect(pt, pt);
    }
    return Rect(Vector2::Min(r.GetMinimum(), pt), Vector2::Max(r.GetMaximum(), pt));
}
Starsurge::Rect Starsurge::Rect::Union(const Rect r, const std::vector<Vector2> pts) {
    Rect ret(r);
    for (unsigned int i = 0; i < pts.size(); ++i) {
        ret = Rect::Union(ret, pts[i]);
    }
    return ret;
}
Starsurge::Rect Starsurge::Rect::Union(const Rect r1, const Rect r2) {
    if (r1.IsNull()) {
        return r2;
    }
    if (r2.IsNull()) {
        return r1;
    }
    return Rect(Vector2::Min(r1.GetMinimum(), r2.GetMinimum()), Vector2::Max(r1.GetMaximum(), r2.GetMaximum()));
}

Starsurge::Rect Starsurge::Rect::Transform(const Matrix2 matrix) {
    if (IsNull()) {
        return Rect::Null();
    }

    Rect ret;
    std::vector<Vector2> corners = GetAllCorners();
    corners[0] = matrix*corners[0];
    corners[1] = matrix*corners[1];
    corners[2] = matrix*corners[2];
    corners[3] = matrix*corners[3];
    return Rect::Union(ret, corners);
}

void Starsurge::Rect::CheckBounds() {
    if ((this->min > this->max).Any()) {
        Log("this = "+ToString());
        Error("For rectangles, the minimum value must be less than or equal to the maximum value.");
        this->isNull = true;
        this->min = this->max;
    }
}
