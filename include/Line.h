#pragma once
#include "Vector.h"

namespace Starsurge {
    class Line {
    public:
        Line();
        Line(Vector3 p1, Vector3 p2, bool inf = false);

        bool Contains(Vector3 test) const;
        Vector3 GetPoint(float t) const;
        float GetT(Vector3 point) const;
        float Length() const;
        float SquaredLength() const;
        float Distance(Vector3 point) const;
        Vector3 ClosestPoint(const Vector3 point) const;

        std::string ToString(unsigned int ndigits = 3) const;

        static Line XAxis();
        static Line YAxis();
        static Line ZAxis();

        Vector3 start;
        Vector3 end;
        bool infinite;

        friend bool operator==(const Line& lhs, const Line& rhs) {
            return ((lhs.start == rhs.start && lhs.end == rhs.end && lhs.infinite == rhs.infinite) || (lhs.start == rhs.end && lhs.end == rhs.start && lhs.infinite == rhs.infinite));
        }
        friend bool operator!=(const Line& lhs, const Line& rhs) { return !(lhs == rhs); }
    };
};
