#include "../include/Sphere.h"

Starsurge::Sphere::Sphere() {
    this->position = Vector3(0,0,0);
    this->radius = 0;
}
Starsurge::Sphere::Sphere(const Sphere& copy) {
    this->position = copy.position;
    this->radius = copy.radius;
}
Starsurge::Sphere::Sphere(Vector3 p, float r) {
    this->position = p;
    this->radius = std::max(0.0f, r);
}

bool Starsurge::Sphere::Contains(const Vector3 pt) const {
    Vector3 diff = pt - this->position;
    float dist = Vector3::Dot(diff, diff);
    return (dist <= this->radius * this->radius);
}
float Starsurge::Sphere::GetVolume() const {
    return (4.0/3.0)*PI*this->radius*this->radius*this->radius;
}
float Starsurge::Sphere::GetSurfaceArea() const {
    return 4*PI*this->radius*this->radius;
}
Starsurge::Vector3 Starsurge::Sphere::ClosestPoint(const Vector3 point) const {
    Vector3 relativePoint = point - this->position;
    relativePoint.Normalize();
    relativePoint *= this->radius;
    return this->position + relativePoint;
}

std::string Starsurge::Sphere::ToString() const {
    std::string ret = "Sphere {";
    if (this->position.x != 0) {
        ret += "(x";
        if (this->position.x < 0) ret += "+"+std::to_string(std::abs(this->position.x));
        else ret += "-"+std::to_string(this->position.x);
        ret += ")^2+";
    }
    else ret += "x^2+";
    if (this->position.x != 0) {
        ret += "(y";
        if (this->position.y < 0) ret += "+"+std::to_string(std::abs(this->position.y));
        else ret += "-"+std::to_string(this->position.y);
        ret += ")^2+";
    }
    else ret += "y^2+";
    if (this->position.z != 0) {
        ret += "(z";
        if (this->position.z < 0) ret += "+"+std::to_string(std::abs(this->position.z));
        else ret += "-"+std::to_string(this->position.z);
        ret += ")^2=";
    }
    else ret += "z^2=";
    ret += std::to_string(this->radius);
    ret += "^2}";
    return ret;
}
