#pragma once
#include "Vector.h"

namespace Starsurge {
    class Line {
    public:
        Line(Vector3 t_start, Vector3 t_end);

        bool Contains(Vector3 test) const;
        Vector3 GetPoint(float t) const;
        float Length() const;
        float SquaredLength() const;
        float Distance(Vector3 point) const;
        Vector3 ClosestPoint(const Vector3 point) const;

        std::string ToString() const;

        Vector3 operator*(float t) const {
            return GetPoint(t);
        }

        Vector3 start;
        Vector3 end;

        friend bool operator==(const Line& lhs, const Line& rhs) {
            return (lhs.start == rhs.start && lhs.end == rhs.end);
        }
        friend bool operator!=(const Line& lhs, const Line& rhs) { return !(lhs == rhs); }
    };
};
