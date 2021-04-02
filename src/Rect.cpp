#include "../include/Rect.h"

Starsurge::Rect::Rect() {
    SetNull();
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
    this->minimum = copy.GetMinimum();
    this->maximum = copy.GetMaximum();
    this->isNull = copy.IsNull();
    CheckBounds();
}
void Starsurge::Rect::SetBounds(Vector2 t_min, Vector2 t_max) {
    this->minimum = t_min;
    this->maximum = t_max;
    this->isNull = false;
    CheckBounds();
}
void Starsurge::Rect::SetBounds(float xmin, float ymin, float xmax, float ymax) {
    this->minimum = Vector2(xmin, ymin);
    this->maximum = Vector2(xmax, ymax);
    this->isNull = false;
    CheckBounds();
}
void Starsurge::Rect::SetBounds(std::vector<Vector2> points) {
    if (points.size() == 0) {
        this->isNull = true;
        this->minimum = Vector2(0,0);
        this->maximum = Vector2(0,0);
        return;
    }

    this->minimum = points[0];
    this->maximum = points[0];
    for (unsigned int i = 1; i < points.size(); ++i) {
        this->minimum = Vector2::Min(this->minimum, points[i]);
        this->maximum = Vector2::Max(this->maximum, points[i]);
    }
    CheckBounds();
}

std::string Starsurge::Rect::ToString(unsigned int ndigits) const {
    if (IsNull()) {
        return "Rect {Null}";
    }
    return "Rect {min = "+this->minimum.ToString(ndigits)+", max = "+this->maximum.ToString(ndigits) + "}";
}

bool Starsurge::Rect::IsNull() const {
    return this->isNull;
}
Starsurge::Vector2 Starsurge::Rect::GetMinimum() const {
    return this->minimum;
}
Starsurge::Vector2 Starsurge::Rect::GetMaximum() const {
    return this->maximum;
}
Starsurge::Vector2 Starsurge::Rect::GetCenter() const {
    return (this->maximum + this->minimum) / 2;
}
Starsurge::Vector2 Starsurge::Rect::GetSize() const {
    return this->maximum - this->minimum;
}
float Starsurge::Rect::GetArea() const {
    Vector2 size = GetSize();
    return size.x*size.y;
}

Starsurge::Vector2 Starsurge::Rect::GetCorner(RectCorner corner) const {
    switch (corner) {
        case TOP_LEFT: return this->minimum;
        case TOP_RIGHT: return Vector2((float)this->maximum.x, (float)this->minimum.y);
        case BOTTOM_LEFT: return Vector2((float)this->minimum.x, (float)this->maximum.y);
        case BOTTOM_RIGHT: return this->maximum;
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
Starsurge::Line2D Starsurge::Rect::GetEdge(RectCorner corner1, RectCorner corner2) const {
    if (corner2 == (corner1-1)%4 || corner2 == (corner1+1)%4) {
        return Line2D(GetCorner(corner1), GetCorner(corner2));
    }
    else {
        Error("Invalid edge in Rect::GetEdge(). Options: (corner1,(corner1-1)%4), (corner1,(corner1+1)%4).");
        return Line2D(Vector2(0,0), Vector2(0,0));
    }
}
std::vector<Starsurge::Line2D> Starsurge::Rect::GetAllEdges() const {
    std::vector<Line2D> ret;
    ret.push_back(GetEdge(TOP_LEFT, TOP_RIGHT));
    ret.push_back(GetEdge(TOP_RIGHT, BOTTOM_RIGHT));
    ret.push_back(GetEdge(BOTTOM_RIGHT, BOTTOM_LEFT));
    ret.push_back(GetEdge(BOTTOM_LEFT, TOP_LEFT));
    return ret;
}

bool Starsurge::Rect::Contains(Vector2 p) const {
    if (IsNull()) {
        return false;
    }

    return (p.x >= this->minimum.x && p.x <= this->maximum.x &&
            p.y >= this->minimum.y && p.y <= this->maximum.y);
}
bool Starsurge::Rect::Contains(Rect r) const {
    if (IsNull() || r.IsNull()) {
        return false;
    }

    Vector2 min = r.GetMinimum();
    Vector2 max = r.GetMaximum();
    return (min.x >= this->minimum.x && max.x <= this->maximum.x &&
            min.y >= this->minimum.y && max.y <= this->maximum.y);
}

Starsurge::Vector2 Starsurge::Rect::ClosestPoint(const Vector2 point) const {
    if (Contains(point)) {
        return point;
    }
    return Vector2::Clamp(point, this->minimum, this->maximum);
}

void Starsurge::Rect::Expand(float amount) {
    Expand(Vector2(amount, amount));
}
void Starsurge::Rect::Expand(Vector2 amount) {
    if (!IsNull()) {
        this->minimum -= amount;
        this->maximum += amount;
    }
}

void Starsurge::Rect::SetNull() {
    this->minimum = Vector2(0,0);
    this->maximum = Vector2(0,0);
    this->isNull = true;
}

Starsurge::Rect Starsurge::Rect::Null() {
    Rect ret;
    return ret;
}
void Starsurge::Rect::Union(const Vector2 pt) {
    if (IsNull()) {
        this->minimum = pt;
        this->maximum = pt;
        this->isNull = false;
        return;
    }
    this->minimum = Vector2::Min(this->minimum, pt);
    this->maximum = Vector2::Max(this->maximum, pt);
}
void Starsurge::Rect::Union(const std::vector<Vector2> pts) {
    for (unsigned int i = 0; i < pts.size(); ++i) {
        Union(pts[i]);
    }
}
void Starsurge::Rect::Union(const Rect r2) {
    if (IsNull()) {
        SetBounds(r2);
        return;
    }
    if (r2.IsNull()) {
        return;
    }
    this->minimum = Vector2::Min(this->minimum, r2.GetMinimum());
    this->maximum = Vector2::Max(this->maximum, r2.GetMaximum());
}
void Starsurge::Rect::Subtract(const Vector2 pt) {
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

    // Get the minimal difference in x,y in the 4 sides.
    float diff1 = pt.x-this->minimum.x;
    float diff2 = pt.y-this->minimum.y;
    float diff3 = this->maximum.x-pt.x;
    float diff4 = this->maximum.y-pt.y;
    float min = Min(Min(diff1,diff2),Min(diff3,diff4));

    // Move whatever face has the least distance to go. This should maximize area.
    float eps = 0.00001; // Add a bit of wiggle room so the point doesn't actually lie in the rectangle anymore.
    if (min == diff1) { this->minimum.x = pt.x+eps; }
    else if (min == diff2) { this->minimum.y = pt.y+eps; }
    else if (min == diff3) { this->maximum.x = pt.x-eps; }
    else { this->maximum.y = pt.y-eps; }
    CheckBounds(false);
}
void Starsurge::Rect::Subtract(const std::vector<Vector2> pts) {
    for (unsigned int i = 0; i < pts.size(); ++i) {
        if (IsNull())
            break;
        Subtract(pts[i]);
    }
}
void Starsurge::Rect::Subtract(const Rect r2) {
    if (IsNull() || r2.IsNull()) {
        return;
    }

    Rect intersection(*this);
    intersection.Intersection(r2);
    if (!intersection.IsNull()) {
        if (Contains(intersection.GetMinimum())) { Subtract(intersection.GetMinimum()); }
        else { Subtract(intersection.GetMaximum()); }
    }
}
void Starsurge::Rect::Intersection(const Rect r2) {
    if (IsNull() || r2.IsNull()) {
        SetNull();
        return;
    }

    if (this->minimum.x <= r2.GetMaximum().x && this->maximum.x >= r2.GetMinimum().x &&
        this->minimum.y <= r2.GetMaximum().y && this->maximum.y >= r2.GetMinimum().y) {
        this->minimum = Vector2::Max(this->minimum, r2.GetMinimum());
        this->maximum = Vector2::Min(this->maximum, r2.GetMaximum());
    }
    else {
        SetNull();
    }
}

void Starsurge::Rect::Transform(const Matrix2 matrix) {
    if (IsNull()) {
        return;
    }

    SetNull();
    std::vector<Vector2> corners = GetAllCorners();
    corners[0] = matrix*corners[0];
    corners[1] = matrix*corners[1];
    corners[2] = matrix*corners[2];
    corners[3] = matrix*corners[3];
    Union(corners);
}

void Starsurge::Rect::CheckBounds(bool raiseError) {
    if ((this->minimum > this->maximum).Any()) {
        if (raiseError) {
            Error("For rectangles, the minimum value must be less than or equal to the maximum value.");
        }
        SetNull();
    }
}
