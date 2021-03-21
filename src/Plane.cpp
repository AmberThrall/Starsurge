#include "../include/Plane.h"

Starsurge::Plane::Plane() {
}

Starsurge::Plane::Plane(const Plane& copy) {
    this->a = copy.a;
    this->b = copy.b;
    this->c = copy.c;
    this->d = copy.d;
}

Starsurge::Plane::Plane(float a, float b, float c, float d) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
}

Starsurge::Plane::Plane(Vector3 n, float d) {
    n.Normalize();
    this->a = n.x;
    this->b = n.y;
    this->c = n.z;
    this->d = d;
}

Starsurge::Plane::Plane(Vector3 n, Vector3 p) {
    n.Normalize();
    this->a = n.x;
    this->b = n.y;
    this->c = n.z;
    this->d = -1.0*Vector3::Dot(n, p);
}

Starsurge::Plane::Plane(Vector3 p1, Vector3 p2, Vector3 p3) {
    Vector3 n = Vector3::CrossProduct(p2 - p1, p3 - p1);
    n.Normalize();
    this->a = n.x;
    this->b = n.y;
    this->c = n.z;
    this->d = -1.0*Vector3::Dot(n, p1);
}

Starsurge::Plane Starsurge::Plane::YZ() {
    return Plane(Vector3(1,0,0), 0);
}
Starsurge::Plane Starsurge::Plane::XZ() {
    return Plane(Vector3(0,1,0), 0);
}
Starsurge::Plane Starsurge::Plane::XY() {
    return Plane(Vector3(0,0,1), 0);
}

Starsurge::Plane Starsurge::Plane::Transform(Matrix4 m) {
    return Plane(
        a*m(0,0) + b*m(1,0) + c*m(2,0),
        a*m(0,1) + b*m(1,1) + c*m(2,1),
        a*m(0,2) + b*m(1,2) + c*m(2,2),
        a*m(0,3) + b*m(1,3) + c*m(2,3) + d
    );
}

bool Starsurge::Plane::Above(Vector3 test) {
    return (Plane::Distance(test) > 0.00001);
}

bool Starsurge::Plane::Below(Vector3 test) {
    return (Plane::Distance(test) < -0.00001);
}

bool Starsurge::Plane::Contains(Vector3 test) {
    return (std::abs(Plane::Distance(test)) < 0.00001);
}

float Starsurge::Plane::Distance(Vector3 p) const {
    return (Vector3::Dot(GetNormal(), p) + this->d) / GetNormal().Norm();
}

void Starsurge::Plane::Normalize() {
    float dist = std::sqrt(Vector3::Dot(GetNormal(), GetNormal()) + this->d*this->d);
    this->a /= dist;
    this->b /= dist;
    this->c /= dist;
    this->d /= dist;
}

Starsurge::Vector3 Starsurge::Plane::ClosestPoint(const Vector3 point) {
    float distance = Vector3::Dot(GetNormal(), point) - this->d;
    distance = distance / Vector3::Dot(GetNormal(), GetNormal());
    return point - distance * GetNormal();
}

Starsurge::Vector3 Starsurge::Plane::GetNormal() const {
    return Vector3(this->a, this->b, this->c);
}

std::string Starsurge::Plane::ToString() const {
    std::string ret = "Plane {"+std::to_string(this->a)+"x ";
    if (this->b < 0) ret += "- "+std::to_string(std::abs(this->b))+"y ";
    else ret += "+ "+std::to_string(this->b)+"y ";
    if (this->c < 0) ret += "- "+std::to_string(std::abs(this->c))+"z ";
    else ret += "+ "+std::to_string(this->c)+"z ";
    if (this->d < 0) ret += "- "+std::to_string(std::abs(this->d));
    else ret += "+ "+std::to_string(this->d);
    ret += " = 0}";
    return ret;
}

Starsurge::Vector3 Starsurge::Plane::Project(const Vector3 v) const {
    return v - Vector3::Dot(v, GetNormal())*GetNormal();
}