#include "../include/Line2D.h"
#include "../include/Utils.h"

Starsurge::Line2D::Line2D() {
    this->start = Vector2(0,0);
    this->end = Vector2(0,0);
    this->infinite = false;
}
Starsurge::Line2D::Line2D(Vector2 p1, Vector2 p2, bool inf) {
    this->start = p1;
    this->end = p2;
    this->infinite = inf;
}

bool Starsurge::Line2D::Contains(Vector2 test) const {
    if (this->start == this->end) {
        return (this->start == test);
    }

    // Solve: start + t*dir = test
    Vector2 dir = this->end - this->start;
    Vector2 d = test-this->start;
    if (!Vector2::Parallel(d, dir)) {
        return false;
    }
    float t = 0;
    if (dir.x != 0) { t = d.x / dir.x; }
    else if (dir.y != 0) { t = d.y / dir.y; }
    else { return false; }

    return ((t >= 0 && t <= 1) || this->infinite);
}
Starsurge::Vector2 Starsurge::Line2D::GetPoint(float t) const {
    if (!this->infinite) {
        t = Clamp(t, 0, 1);
    }
    Vector2 dir = this->end - this->start;
    return this->start + t*dir;
}
float Starsurge::Line2D::GetT(Vector2 point) const {
    if (!Contains(point)) {
        return -1;
    }
    // Solve: point = start+t*dir
    Vector2 dir = this->end - this->start;
    if (dir.x != 0) { return (point.x - this->start.x) / dir.x; }
    if (dir.y != 0) { return (point.y - this->start.y) / dir.y; }
    return -1;
}
float Starsurge::Line2D::Length() const {
    if (this->infinite) {
        return Infinity();
    }
    return Sqrt(SquaredLength());
}
float Starsurge::Line2D::SquaredLength() const {
    if (this->infinite) {
        return Infinity();
    }
    return (this->end - this->start).SquaredLength();
}
float Starsurge::Line2D::Distance(Vector2 point) const {
    Vector2 closest = ClosestPoint(point);
    return (point-closest).Magnitude();
}
Starsurge::Vector2 Starsurge::Line2D::ClosestPoint(const Vector2 point) const {
    if (this->end == this->start) {
        return this->start;
    }
    Vector2 u = this->start - point;
    Vector2 v = this->end - this->start;
    float t = -(Vector2::Dot(v,u))/Vector2::Dot(v,v);
    return GetPoint(t);
}
float Starsurge::Line2D::GetSlope() const {
    return (this->end.y - this->start.y) / (this->end.x - this->start.x);
}
float Starsurge::Line2D::GetYIntercept() const {
    return this->start.y-GetSlope()*this->start.x;
}

std::string Starsurge::Line2D::ToString(unsigned int ndigits) const {
    std::string ret = "Line2D {"+this->start.ToString(ndigits)+"+t"+(this->end-this->start).ToString(ndigits);
    if (this->infinite) {
        ret += ", -Inf<t<Inf}";
    }
    else {
        ret += ", 0<=t<=1}";
    }
    return ret;
}

Starsurge::Line2D Starsurge::Line2D::XAxis() {
    return Line2D(Vector2(0,0), Vector2(1,0), true);
}
Starsurge::Line2D Starsurge::Line2D::YAxis() {
    return Line2D(Vector2(0,0), Vector2(0,1), true);
}
