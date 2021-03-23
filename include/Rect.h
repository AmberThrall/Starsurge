#pragma once
#include <vector>
#include "Vector.h"
#include "Matrix.h"

namespace Starsurge {
    enum RectCorner {
        TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT
    };

    class Rect {
    public:
        Rect();
        Rect(const Rect& copy);
        Rect(Vector2 t_min, Vector2 t_max);
        Rect(float xmin, float ymin, float xmax, float ymax);
        Rect(std::vector<Vector2> points);

        void SetBounds(const Rect& copy);
        void SetBounds(Vector2 t_min, Vector2 t_max);
        void SetBounds(float xmin, float ymin, float xmax, float ymax);
        void SetBounds(std::vector<Vector2> points);

        std::string ToString() const;

        bool IsNull() const;
        Vector2 GetMinimum() const;
        Vector2 GetMaximum() const;
        Vector2 GetCenter() const;
        Vector2 GetSize() const;
        float GetArea() const;
        Vector2 GetCorner(RectCorner corner) const;
        std::vector<Vector2> GetAllCorners() const;
        bool Contains(Vector2 p) const;
        bool Contains(Rect r) const;
        Vector2 ClosestPoint(const Vector2 point) const;

        Rect Expand(float amount);
        Rect Expand(Vector2 amount);

        static Rect Null();
        static Rect Union(const Rect r, const Vector2 pt);
        static Rect Union(const Rect r, const std::vector<Vector2> pts);
        static Rect Union(const Rect r1, const Rect r2);

        Rect Transform(const Matrix2 matrix);

        Rect& operator=(const Rect& other) {
            if (this != &other) {
                SetBounds(other);
            }
            return *this;
        }
        Rect& operator+=(const Rect& rhs) {
            if (IsNull() || rhs.IsNull()) {
                return *this;
            }
            this->min += rhs.GetMinimum();
            this->max += rhs.GetMaximum();
            return *this;
        }
        Rect& operator+=(const Vector2& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->min += rhs;
            this->max += rhs;
            return *this;
        }
        friend Rect operator+(Rect lhs, const Rect& rhs) { return lhs += rhs; }
        friend Rect operator+(Rect lhs, const Vector2& rhs) { return lhs += rhs; }
        Rect& operator-=(const Rect& rhs) {
            if (IsNull() || rhs.IsNull()) {
                return *this;
            }
            this->min -= rhs.GetMinimum();
            this->max -= rhs.GetMaximum();
            return *this;
        }
        Rect& operator-=(const Vector2& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->min -= rhs;
            this->max -= rhs;
            return *this;
        }
        friend Rect operator-(Rect rhs) { return -1.0f * rhs; }
        friend Rect operator-(Rect lhs, const Rect& rhs) { return lhs -= rhs; }
        friend Rect operator-(Rect lhs, const Vector2& rhs) { return lhs -= rhs; }
        Rect& operator*=(const float& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->min *= rhs;
            this->max *= rhs;
            return *this;
        }
        friend Rect operator*(Rect lhs, const float rhs) { return lhs *= rhs; }
        friend Rect operator*(const float lhs, Rect rhs) { return rhs *= lhs; }
        Rect& operator/=(const float& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->min /= rhs;
            this->max /= rhs;
            return *this;
        }
        friend Rect operator/(Rect lhs, const float rhs) { return lhs /= rhs; }
        friend bool operator==(const Rect& lhs, const Rect& rhs) {
            if (lhs.IsNull()) {
                return rhs.IsNull();
            }
            if (rhs.IsNull()) {
                return false;
            }
            return (lhs.GetMinimum() == rhs.GetMinimum() && lhs.GetMaximum() == rhs.GetMaximum());
        }
        friend bool operator!=(const Rect& lhs, const Rect& rhs) { return !(lhs == rhs); }
    private:
        Vector2 min;
        Vector2 max;
        bool isNull;
        void CheckBounds();
    };
}
