#include "../include/Circle.h"
#include "../include/Utils.h"

Starsurge::Circle::Circle() {
    this->Position = Vector2(0,0);
    SetRadius(0);
}
Starsurge::Circle::Circle(const Circle& copy) {
    this->Position = copy.Position;
    SetRadius(copy.GetRadius());
}
Starsurge::Circle::Circle(Vector2 p, float r) {
    this->Position = p;
    SetRadius(r);
}

void Starsurge::Circle::SetRadius(float r) {
    this->radius = Max(0, r);
}

bool Starsurge::Circle::Contains(const Vector2 pt) const {
    return ((pt - this->Position).SquaredLength() < this->radius*this->radius);
}
float Starsurge::Circle::GetArea() const {
    return PI*this->radius*this->radius;
}
float Starsurge::Circle::GetRadius() const {
    return this->radius;
}
Starsurge::Vector2 Starsurge::Circle::ClosestPoint(const Vector2 point) const {
    if (Vector2::IsSame(point, this->Position)) {
        return this->Position + Vector2(this->radius,0);
    }
    Vector2 relativePoint = point - this->Position;
    relativePoint.Normalize();
    relativePoint *= this->radius;
    return this->Position + relativePoint;
}
float Starsurge::Circle::Distance(const Vector2 point) const {
    Vector2 closest = ClosestPoint(point);
    return (closest - this->Position).Magnitude();
}

std::string Starsurge::Circle::ToString(unsigned int ndigits) const {
    std::string ret = "Circle {";
    if (this->Position.x != 0) {
        ret += "(x";
        if (this->Position.x < 0) ret += "+"+Starsurge::ToString(Abs(this->Position.x), ndigits, false);
        else ret += "-"+Starsurge::ToString(this->Position.x, ndigits, false);
        ret += ")^2+";
    }
    else ret += "x^2+";
    if (this->Position.y != 0) {
        ret += "(y";
        if (this->Position.y < 0) ret += "+"+Starsurge::ToString(Abs(this->Position.y), ndigits, false);
        else ret += "-"+Starsurge::ToString(this->Position.y, ndigits, false);
        ret += ")^2=";
    }
    else ret += "y^2=";
    ret += Starsurge::ToString(this->radius, ndigits, false);
    ret += "^2}";
    return ret;
}
