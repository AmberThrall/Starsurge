#pragma once
#include <initializer_list>
#include <vector>
#include <iostream>
#include "Engine.h"

namespace Starsurge {
    class Vector {
    public:
        Vector(size_t n, float t_val = 0);
        Vector(const Vector& other);
        Vector(std::vector<float> t_data);
        Vector(std::initializer_list<float> list);

        size_t GetSize() const;
        void Resize(size_t n);
        std::string ToString() const;

        float Length() const;
        float Magnitude() const;
        void Normalize();
        Vector Unit() const;
        static float Dot(const Vector& lhs, const Vector& rhs);
        static Vector Basis(size_t n, size_t i);
        static Vector CrossProduct(const Vector& a, const Vector& b);

        // Operators:
        Vector& operator=(const Vector& other) {
            if (this != &other) {
                this->data.clear();
                for (size_t i = 0; i < other.data.size(); ++i) {
                    this->data.push_back(other[i]);
                }
            }
            return *this;
        }
        float operator [](int i) const { return this->data[i]; }
        float & operator [](int i) { return this->data[i]; }
        Vector& operator+=(const Vector& rhs) {
            if (this->GetSize() != rhs.GetSize()) { // Ensure addition makes sense.
                Error("Can't add vectors of varying size.");
                return *this;
            }
            for (size_t i=0; i < this->GetSize(); ++i) {
                this->data[i] += rhs[i];
            }
            return *this;
        }
        friend Vector operator+(Vector lhs, const Vector& rhs)  {
            lhs += rhs;
            return lhs;
        }
        Vector& operator-=(const Vector& rhs) {
            if (this->GetSize() != rhs.GetSize()) { // Ensure addition makes sense.
                Error("Can't subtract vectors of varying size.");
                return *this;
            }
            for (size_t i=0; i < this->GetSize(); ++i) {
                this->data[i] -= rhs[i];
            }
            return *this;
        }
        friend Vector operator-(Vector lhs, const Vector& rhs)  {
            lhs -= rhs;
            return lhs;
        }
        friend bool operator==(const Vector& lhs, const Vector& rhs) { return lhs.data == rhs.data; }
        friend bool operator!=(const Vector& lhs, const Vector& rhs) { return !(lhs == rhs); }

    private:
        std::vector<float> data;
    };
}
