#pragma once
#include <string>
#include <initializer_list>
#include "Logging.h"

namespace Starsurge {
    template<size_t N>
    class Vector {
    public:
        Vector(float t_val = 0) {
            for (size_t i = 0; i < N; ++i) {
                this->data[i] = t_val;
            }
        }
        Vector(const Vector<N>& other) {
            for (size_t i = 0; i < N; ++i) {
                this->data[i] = other.data[i];
            }
        }
        Vector(float t_data[N]) {
            for (size_t i = 0; i < N; ++i) {
                this->data[i] = t_data[i];
            }
        }
        Vector(std::initializer_list<float> list) {
            if (list.size() != N) {
                Error("Not correct amount of data.");
                for (size_t i = 0; i < N; ++i) {
                    this->data[i] = 0;
                }
                return;
            }
            size_t i = 0;
            for (auto it = std::begin(list); it != std::end(list); ++it) {
                this->data[i] = *it;
                i++;
            }
        }

        size_t GetSize() const { return N; }

        template <size_t M>
        Vector<M> Resize() {
            Vector<M> ret;
            for (size_t i = 0; i < M && i < N; ++i) {
                ret[i] = this->data[i];
            }
            return ret;
        }
        std::string ToString() const {
            std::string ret = "[";
            for (size_t i = 0; i < N; ++i) {
                if (i > 0)
                    ret += ",";
                ret += std::to_string(this->data[i]);
            }
            ret += "]";
            return ret;
        }

        float Length() const { return Magnitude(); }
        float Magnitude() const {
            return std::sqrt(Vector::Dot(*this, *this));
        }
        void Normalize() {
            float mag = Magnitude();
            if (mag == 0) {
                Error("Can't normalize zero vector.");
                return;
            }
            for (size_t i = 0; i < N; ++i) {
                this->data[i] *= (1.0/mag);
            }
        }
        Vector Unit() const {
            Vector<N> copy(*this);
            copy.Normalize();
            return copy;
        }

        static float Dot(const Vector<N>& lhs, const Vector<N>& rhs) {
            float ret = 0;
            for (size_t i = 0; i < N; ++i) {
                ret += lhs[i]*rhs[i];
            }
            return ret;
        }
        static Vector<N> Basis(size_t i) {
            Vector<N> ret;
            ret[i] = 1;
            return ret;
        }

        // Operators:
        Vector<N>& operator=(const Vector<N>& other) {
            if (this != &other) {
                for (size_t i = 0; i < N; ++i) {
                    this->data[i] = other.data[i];
                }
            }
            return *this;
        }
        float operator [](int i) const { return this->data[i]; }
        float & operator [](int i) { return this->data[i]; }
        Vector<N>& operator+=(const Vector<N>& rhs) {
            for (size_t i=0; i < N; ++i) {
                this->data[i] += rhs[i];
            }
            return *this;
        }
        friend Vector<N> operator+(Vector<N> lhs, const Vector<N>& rhs) { return lhs += rhs; }
        Vector<N>& operator-=(const Vector<N>& rhs) {
            for (size_t i=0; i < N; ++i) {
                this->data[i] -= rhs[i];
            }
            return *this;
        }
        friend Vector<N> operator-(Vector<N> lhs, const Vector<N>& rhs) { return lhs -= rhs; }
        Vector<N>& operator*=(const float& rhs) {
            for (size_t i=0; i < N; ++i) {
                this->data[i] *= rhs;
            }
            return *this;
        }
        friend Vector<N> operator*(Vector<N> lhs, const float& rhs) { return lhs *= rhs; }
        friend Vector<N> operator*(const float& lhs, Vector<N> rhs) { return rhs *= lhs; }
        friend bool operator==(const Vector<N>& lhs, const Vector<N>& rhs) { return lhs.data == rhs.data; }
        friend bool operator!=(const Vector<N>& lhs, const Vector<N>& rhs) { return !(lhs == rhs); }
    protected:
        float data[N];
    };

    class Vector2 : public Vector<2> {
    public:
        Vector2(float t_val = 0) : Vector<2>(t_val) {}
        Vector2(const Vector<2>& other) : Vector<2>(other) {}
        Vector2(float t_data[2]) : Vector<2>(t_data) {}
        Vector2(std::initializer_list<float> list) : Vector<2>(list) {}
        Vector2(float x, float y) {
            this->data[0] = x;
            this->data[1] = y;
        }

        static float Dot(Vector2 a, Vector2 b) { return Vector<2>::Dot(a, b); }
    };

    class Vector3 : public Vector<3> {
    public:
        Vector3(float t_val = 0) : Vector<3>(t_val) {}
        Vector3(const Vector<3>& other) : Vector<3>(other) {}
        Vector3(float t_data[3]) : Vector<3>(t_data) {}
        Vector3(std::initializer_list<float> list) : Vector<3>(list) {}
        Vector3(float x, float y, float z) {
            this->data[0] = x;
            this->data[1] = y;
            this->data[2] = z;
        }

        static float Dot(Vector3 a, Vector3 b) { return Vector<3>::Dot(a, b); }
        static Vector3 CrossProduct(Vector3 a, Vector3 b);
    };

    class Vector4 : public Vector<4> {
    public:
        Vector4(float t_val = 0) : Vector<4>(t_val) {}
        Vector4(const Vector<4>& other) : Vector<4>(other) {}
        Vector4(float t_data[4]) : Vector<4>(t_data) {}
        Vector4(std::initializer_list<float> list) : Vector<4>(list) {}
        Vector4(float x, float y, float z, float w) {
            this->data[0] = x;
            this->data[1] = y;
            this->data[2] = z;
            this->data[3] = w;
        }

        static float Dot(Vector4 a, Vector4 b) { return Vector<4>::Dot(a, b); }
    };
}
