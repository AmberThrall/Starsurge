#include "../include/Line.h"

Starsurge::Line::Line(Vector3 t_start, Vector3 t_end) {
    this->start = t_start;
    this->end = t_end;
}

bool Starsurge::Line::Contains(Vector3 test) const {
    return (Distance(test) < 0.00001);
}

Starsurge::Vector3 Starsurge::Line::GetPoint(float t) const {
    t = Clamp(t, 0, 1);
    Vector3 dir = this->end - this->start;
    return this->start + t*dir;
}

float Starsurge::Line::Length() const {
    return (this->end-this->start).Magnitude();
}

float Starsurge::Line::SquaredLength() const {
    return (this->end-this->start).SquaredMagnitude();
}

std::string Starsurge::Line::ToString() const {
    return "Line {start = "+this->start.ToString()+", end = "+this->end.ToString()+"}";
}

float Starsurge::Line::Distance(Vector3 point) const {
    Vector3 closest = ClosestPoint(point);
    return (point - closest).Magnitude();
}

Starsurge::Vector3 Starsurge::Line::ClosestPoint(const Vector3 point) const {
    Vector3 d = this->start - point;
    Vector3 dir = (this->end - this->start).Unit();
    float lhs = Vector3::Dot(2*dir, dir);
    float rhs = Vector3::Dot(-2*dir, d);
    float t = rhs / lhs;
    return GetPoint(t);
}
