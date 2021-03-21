#include "../include/Line.h"

Starsurge::Line::Line(Vector3 p, Vector3 v) {
    this->origin = p;
    this->direction = v;
}

bool Starsurge::Line::Contains(Vector3 test) {
    return Vector3::Parallel(test-this->origin, this->direction);
}

Starsurge::Vector3 Starsurge::Line::GetPoint(float t) const {
    return this->origin + t*this->direction;
}

std::string Starsurge::Line::ToString() {
    return "Line {"+this->origin.ToString()+" + t"+this->direction.ToString()+"}";
}

float Starsurge::Line::Distance(Vector3 p) const {
    Vector3 v = (this->origin - p)-Vector3::Dot(this->origin - p, this->direction)*this->direction;
    return v.Norm();
}

Starsurge::Vector3 Starsurge::Line::ClosestPoint(const Vector3 point) {
    // A = <this->origin.x + this->direction.x*t, this->origin.y + this->direction.y*t, this->origin.z + this->direction.z*t>
    // d^2 = (this->origin.x + this->direction.x*t - point.x)^2 + (this->origin.y + this->direction.y*t - point.y)^2 + ...
    // (d^2)' = 2*this->direction.x*(this->origin.x - point.x + this->direction.x*t) + ... = 0
    // 2*this->direction.x*(this->origin.x - point.x) + 2*this->direction.x*this->direction.x*t + ... = 0
    // t(2*this->direction.x*this->direction.x+...) = -2*this->direction.x*(this->origin.x-point.x) - ...
    // t = (-2*this->direction.x*(this->origin.x-point.x)-...)/(2*this->direction.x*this->direction.x+...)
    Vector3 d = this->origin - point;
    float lhs = Vector3::Dot(2*this->direction, this->direction);
    float rhs = Vector3::Dot(-2*this->direction, d);
    float t = rhs / lhs;
    return GetPoint(t);
}
