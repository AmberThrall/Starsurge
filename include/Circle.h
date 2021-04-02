#pragma once
#include "Vector.h"

namespace Starsurge {
    class Circle {
    public:
        Circle();
        Circle(const Circle& copy);
        Circle(Vector2 p, float r);

        void SetRadius(float r);

        bool Contains(const Vector2 pt) const;
        float GetArea() const;
        float GetRadius() const;
        Vector2 ClosestPoint(const Vector2 point) const;
        float Distance(const Vector2 point) const;

        std::string ToString(unsigned int ndigits = 3) const;

        Vector2 Position;
    private:
        float radius;
    };
};
