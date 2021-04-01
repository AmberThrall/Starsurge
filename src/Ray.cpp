#include "../include/Ray.h"
#include "../include/Matrix.h"
#include "../include/Line.h"

Starsurge::Ray::Ray(Vector3 p, Vector3 v) {
    this->origin = p;
    this->direction = v;
    this->direction.Normalize();
}

bool Starsurge::Ray::Contains(Vector3 test) const {
    if (test == this->origin) {
        return true;
    }

    if (Vector3::SameDirection(test-this->origin, this->direction)) {
        return true;
    }
    return false;
}

Starsurge::Vector3 Starsurge::Ray::GetPoint(float t) const {
    if (t < 0) {
        return this->origin;
    }
    return this->origin + t*this->direction;
}

std::string Starsurge::Ray::ToString() const {
    return "Ray {"+this->origin.ToString()+" + t"+this->direction.ToString()+", t>=0}";
}

float Starsurge::Ray::Distance(Vector3 p) const {
    float dot = Vector3::Dot(this->origin - p, this->direction);
    if (dot > 0) {
        Vector3 v = (this->origin - p)-dot*this->direction;
        return v.Norm();
    }

    Vector3 v = (this->origin - p);
    return v.Norm();
}

float Starsurge::Ray::GetT(Vector3 point) const {
    if (!Contains(point)) {
        return -1;
    }
    // point = origin+t*dir
    if (this->direction.x != 0) {
        return (point.x - this->origin.x) / this->direction.x;
    }
    if (this->direction.y != 0) {
        return (point.y - this->origin.y) / this->direction.y;
    }
    if (this->direction.z != 0) {
        return (point.z - this->origin.z) / this->direction.z;
    }
    return -1;
}

Starsurge::Vector3 Starsurge::Ray::ClosestPoint(const Vector3 point) const {
    Line asline(this->origin, this->direction);
    Vector3 closest = asline.ClosestPoint(point);
    if (!Contains(closest)) {
        return this->origin;
    }
    return closest;
}
