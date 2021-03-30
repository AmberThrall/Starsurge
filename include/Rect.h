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

        std::string ToString(unsigned int ndigits = 3) const;

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

        void Expand(float amount);
        void Expand(Vector2 amount);
        void SetNull();

        static Rect Null();
        void Union(const Vector2 pt);
        void Union(const std::vector<Vector2> pts);
        void Union(const Rect r2);
        void Subtract(const Vector2 pt);
        void Subtract(const std::vector<Vector2> pts);
        void Subtract(const Rect r2);
        void Intersection(const Rect r2);

        void Transform(const Matrix2 matrix);

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
            this->minimum += rhs.GetMinimum();
            this->maximum += rhs.GetMaximum();
            CheckBounds(false);
            return *this;
        }
        Rect& operator+=(const Vector2& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum += rhs;
            this->maximum += rhs;
            CheckBounds(false);
            return *this;
        }
        friend Rect operator+(Rect lhs, const Rect& rhs) { return lhs += rhs; }
        friend Rect operator+(Rect lhs, const Vector2& rhs) { return lhs += rhs; }
        Rect& operator-=(const Rect& rhs) {
            if (IsNull() || rhs.IsNull()) {
                return *this;
            }
            this->minimum -= rhs.GetMinimum();
            this->maximum -= rhs.GetMaximum();
            CheckBounds(false);
            return *this;
        }
        Rect& operator-=(const Vector2& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum -= rhs;
            this->maximum -= rhs;
            CheckBounds(false);
            return *this;
        }
        friend Rect operator-(Rect rhs) { return -1.0f * rhs; }
        friend Rect operator-(Rect lhs, const Rect& rhs) { return lhs -= rhs; }
        friend Rect operator-(Rect lhs, const Vector2& rhs) { return lhs -= rhs; }
        Rect& operator*=(const float& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum *= rhs;
            this->maximum *= rhs;
            CheckBounds(false);
            return *this;
        }
        friend Rect operator*(Rect lhs, const float rhs) { return lhs *= rhs; }
        friend Rect operator*(const float lhs, Rect rhs) { return rhs *= lhs; }
        Rect& operator/=(const float& rhs) {
            if (IsNull()) {
                return *this;
            }
            this->minimum /= rhs;
            this->maximum /= rhs;
            CheckBounds(false);
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
        Vector2 minimum;
        Vector2 maximum;
        bool isNull;
        void CheckBounds(bool raiseError = true);
    };
}
