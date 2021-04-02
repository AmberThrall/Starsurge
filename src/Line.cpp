#include "../include/Line.h"

Starsurge::Line::Line() {
    this->start = Vector3(0,0,0);
    this->end = Vector3(0,0,0);
    this->infinite = false;
}
Starsurge::Line::Line(Vector3 p1, Vector3 p2, bool inf) {
    this->start = p1;
    this->end = p2;
    this->infinite = inf;
}

bool Starsurge::Line::Contains(Vector3 test) const {
    if (this->start == this->end) {
        return (this->start == test);
    }

    Log("Is "+test.ToString()+" on "+ToString()+"?");

    // Solve: start + t*dir = test
    Vector3 dir = this->end - this->start;
    Vector3 d = test-this->start;
    if (!Vector3::Parallel(d, dir)) {
        return false;
    }
    float t = 0;
    if (dir.x != 0) { t = d.x / dir.x; }
    else if (dir.y != 0) { t = d.y / dir.y; }
    else if (dir.z != 0) { t = d.z / dir.z; }
    else { return false; }

    return ((t >= 0 && t <= 1) || this->infinite);
}

Starsurge::Vector3 Starsurge::Line::GetPoint(float t) const {
    if (!this->infinite) {
        t = Clamp(t, 0, 1);
    }
    Vector3 dir = this->end - this->start;
    return this->start + t*dir;
}

float Starsurge::Line::GetT(Vector3 point) const {
    if (!Contains(point)) {
        return 0;
    }
    // point = start+t*dir
    Vector3 dir = this->end - this->start;
    if (dir.x != 0) { return (point.x - this->start.x) / dir.x; }
    if (dir.y != 0) { return (point.y - this->start.y) / dir.y; }
    if (dir.z != 0) { return (point.z - this->start.z) / dir.z; }
    return 0;
}

float Starsurge::Line::Length() const {
    if (this->infinite) {
        return Infinity();
    }
    return (this->end-this->start).Magnitude();
}

float Starsurge::Line::SquaredLength() const {
    if (this->infinite) {
        return Infinity();
    }
    return (this->end-this->start).SquaredMagnitude();
}

std::string Starsurge::Line::ToString(unsigned int ndigits) const {
    std::string ret = "Line {"+this->start.ToString(ndigits)+"+t"+(this->end-this->start).ToString(ndigits);
    if (this->infinite) {
        ret += ", -Inf<t<Inf}";
    }
    else {
        ret += ", 0<=t<=1}";
    }
    return ret;
}

float Starsurge::Line::Distance(Vector3 point) const {
    Vector3 closest = ClosestPoint(point);
    return (point - closest).Magnitude();
}

Starsurge::Vector3 Starsurge::Line::ClosestPoint(const Vector3 point) const {
    if (this->end == this->start) {
        return this->start;
    }
    Vector3 u = this->start - point;
    Vector3 v = this->end - this->start;
    float t = -(Vector3::Dot(v,u))/Vector3::Dot(v,v);
    return GetPoint(t);
}

Starsurge::Line Starsurge::Line::XAxis() {
    return Line(Vector3(0,0,0), Vector3(1,0,0), true);
}
Starsurge::Line Starsurge::Line::YAxis() {
    return Line(Vector3(0,0,0), Vector3(0,1,0), true);
}
Starsurge::Line Starsurge::Line::ZAxis() {
    return Line(Vector3(0,0,0), Vector3(0,0,1), true);
}
