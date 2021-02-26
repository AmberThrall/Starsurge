#include "../include/Vector.h"
#include <string>

Starsurge::Vector::Vector(size_t n, float t_val) {
    this->Resize(n);
    for (size_t i = 0; i < n; ++i) {
        this->data[i] = t_val;
    }
}

Starsurge::Vector::Vector(const Vector& other) {
    for (size_t i = 0; i < other.GetSize(); ++i) {
        this->data.push_back(other.data[i]);
    }
}

Starsurge::Vector::Vector(std::vector<float> t_data) {
    for (size_t i = 0; i < t_data.size(); ++i) {
        this->data.push_back(t_data[i]);
    }
}

Starsurge::Vector::Vector(std::initializer_list<float> list) {
    for (auto it = std::begin(list); it != std::end(list); ++it) {
        this->data.push_back(*it);
    }
}

size_t Starsurge::Vector::GetSize() const {
    return this->data.size();
}

void Starsurge::Vector::Resize(size_t n) {
    this->data.resize(n);
}

std::string Starsurge::Vector::ToString() const {
    std::string ret = "[";
    for (size_t i = 0; i < this->data.size(); ++i) {
        if (i > 0)
            ret += ",";
        ret += std::to_string(this->data[i]);
    }
    ret += "]";
    return ret;
}

float Starsurge::Vector::Length() const {
    return Magnitude();
}

float Starsurge::Vector::Magnitude() const {
    return std::sqrt(Vector::Dot(*this, *this));
}

void Starsurge::Vector::Normalize() {
    float mag = Magnitude();
    for (size_t i = 0; i < GetSize(); ++i) {
        this->data[i] *= (1.0/mag);
    }
}

Starsurge::Vector Starsurge::Vector::Unit() const {
    Vector copy(*this);
    copy.Normalize();
    return copy;
}

float Starsurge::Vector::Dot(const Vector& lhs, const Vector& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) { // Ensure dot product makes sense.
        Error("Can't take dot product of two vectors of varying size.");
        return 0;
    }
    float ret = 0;
    for (size_t i = 0; i < lhs.GetSize(); ++i) {
        ret += lhs[i]*rhs[i];
    }
    return ret;
}

Starsurge::Vector Starsurge::Vector::Basis(size_t n, size_t i) {
    Vector ret(n, 0);
    ret[i] = 1;
    return ret;
}

Starsurge::Vector Starsurge::Vector::CrossProduct(const Vector& a, const Vector& b) {
    if (a.GetSize() != 3 || b.GetSize() != 3) { // Ensure cross product makes sense.
        Error("Cross product requires both vectors have size 3.");
        return Vector(0,0);
    }
    std::vector<float> res;
    res.push_back(a[1]*b[2] - a[2]*b[1]);
    res.push_back(-(a[0]*b[2] - a[2]*b[0]));
    res.push_back(a[0]*b[1] - a[1]*b[0]);
    return Vector(res);
}
