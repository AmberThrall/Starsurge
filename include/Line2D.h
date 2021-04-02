#pragma once
#include "Vector.h"

namespace Starsurge {
    class Line2D {
    public:
        Line2D();
        Line2D(Vector2 p1, Vector2 p2, bool inf = false);

        bool Contains(Vector2 test) const;
        Vector2 GetPoint(float t) const;
        float GetT(Vector2 point) const;
        float Length() const;
        float SquaredLength() const;
        float Distance(Vector2 point) const;
        Vector2 ClosestPoint(const Vector2 point) const;
        float GetSlope() const;
        float GetYIntercept() const;

        std::string ToString(unsigned int ndigits = 3) const;

        static Line2D XAxis();
        static Line2D YAxis();

        Vector2 start;
        Vector2 end;
        bool infinite;

        friend bool operator==(const Line2D& lhs, const Line2D& rhs) {
            return ((lhs.start == rhs.start && lhs.end == rhs.end && lhs.infinite == rhs.infinite) || (lhs.start == rhs.end && lhs.end == rhs.start && lhs.infinite == rhs.infinite));
        }
        friend bool operator!=(const Line2D& lhs, const Line2D& rhs) { return !(lhs == rhs); }
    };
};
