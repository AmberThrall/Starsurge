#pragma once
#include <string>
#include <initializer_list>
#include "Logging.h"

namespace Starsurge {
    struct Empty {};

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
        template <typename... Ts, typename = std::enable_if_t<(sizeof...(Ts)==N)>>
        Vector(Ts... ts) {
            size_t i = 0;
            ((this->data[i++] = float(ts)),...);
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
            *this *= (1.0/mag);
        }
        Vector<N> Unit() const {
            Vector<N> copy(*this);
            copy.Normalize();
            return copy;
        }

        template <typename = std::enable_if_t<(N==3)>>
        static Vector<3> Up() {
            return Vector<3>(0,1,0);
        }

        static Vector<N> Normalize(const Vector<N>& v) {
            Vector<N> ret = v;
            ret.Normalize();
            return ret;
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
        static Vector<3> CrossProduct(Vector<3> a, Vector<3> b) {
            float x = a[1]*b[2] - a[2]*b[1];
            float y = -(a[0]*b[2] - a[2]*b[0]);
            float z = a[0]*b[1] - a[1]*b[0];
            return Vector<3>(x, y, z);
        }

        void Set(size_t i, float v) {
            this->data[i] = v;
        }
        float Get(size_t i) {
            return this->data[i];
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
        Vector<N>& operator/=(const float& rhs) {
            for (size_t i=0; i < N; ++i) {
                this->data[i] /= rhs;
            }
            return *this;
        }
        friend Vector<N> operator/(Vector<N> lhs, const float& rhs) { return lhs /= rhs; }
        friend Vector<N> operator/(const float& lhs, Vector<N> rhs) { return rhs /= lhs; }
        friend bool operator==(const Vector<N>& lhs, const Vector<N>& rhs) {
            for (size_t i=0; i < N; ++i) {
                if (lhs[i] != rhs[i]) {
                    return false;
                }
            }
            return true;
        }
        friend bool operator!=(const Vector<N>& lhs, const Vector<N>& rhs) { return !(lhs == rhs); }

        // Swizzling
        template <size_t...Idx>
        class swizzle {
        public:
            std::string ToString() {
                return ((Vector<sizeof...(Idx)>)*this).ToString();
            }

            operator Vector<sizeof...(Idx)>() const {
                int i = 0;
                Vector<sizeof...(Idx)> ret;
                ((ret[i++] = m_data[Idx]),...);
                return ret;
            }
            template <size_t...UIdx, typename = std::enable_if_t<(sizeof...(Idx)==sizeof...(UIdx))>>
            auto operator=(const swizzle<UIdx...>& other) {
                ((m_data[Idx] = other.m_data[UIdx]),...);
                return (*this);
            }
            auto operator=(const Vector<sizeof...(Idx)>& other) {
                int i = 0;
                ((m_data[Idx] = other[i++]),...);
                return (*this);
            }
            template <size_t...UIdx, typename = std::enable_if_t<(sizeof...(Idx)==sizeof...(UIdx))>>
            auto operator+=(const swizzle<UIdx...>& other) {
                ((m_data[Idx] += other.m_data[UIdx]),...);
                return (*this);
            }
            auto operator+=(const Vector<sizeof...(Idx)>& other) {
                int i = 0;
                ((m_data[Idx] += other[i++]),...);
                return (*this);
            }
            template <size_t...UIdx, typename = std::enable_if_t<(sizeof...(Idx)==sizeof...(UIdx))>>
            friend swizzle<Idx...> operator+(swizzle<Idx...> lhs, swizzle<UIdx...> rhs) { return lhs += rhs; }
            friend swizzle<Idx...> operator+(swizzle<Idx...> lhs, Vector<sizeof...(Idx)> rhs) { return lhs += rhs; }
            friend swizzle<Idx...> operator+(Vector<sizeof...(Idx)> lhs, swizzle<Idx...> rhs) { return rhs += lhs; }
            template <size_t...UIdx, typename = std::enable_if_t<(sizeof...(Idx)==sizeof...(UIdx))>>
            auto operator-=(const swizzle<UIdx...>& other) {
                ((m_data[Idx] -= other.m_data[UIdx]),...);
                return (*this);
            }
            auto operator-=(const Vector<sizeof...(Idx)>& other) {
                int i = 0;
                ((m_data[Idx] -= other[i++]),...);
                return (*this);
            }
            template <size_t...UIdx, typename = std::enable_if_t<(sizeof...(Idx)==sizeof...(UIdx))>>
            friend swizzle<Idx...> operator-(swizzle<Idx...> lhs, swizzle<UIdx...> rhs) { return lhs -= rhs; }
            friend swizzle<Idx...> operator-(swizzle<Idx...> lhs, Vector<sizeof...(Idx)> rhs) { return lhs -= rhs; }
            friend swizzle<Idx...> operator-(Vector<sizeof...(Idx)> lhs, swizzle<Idx...> rhs) { return rhs -= lhs; }
            auto operator*=(const float& other) {
                ((m_data[Idx] *= other),...);
                return (*this);
            }
            friend swizzle<Idx...> operator*(swizzle<Idx...> lhs, float rhs) { return lhs *= rhs; }
            friend swizzle<Idx...> operator*(float lhs, swizzle<Idx...> rhs) { return rhs *= lhs; }
            auto operator/=(const float& other) {
                ((m_data[Idx] /= other),...);
                return (*this);
            }
            friend swizzle<Idx...> operator/(swizzle<Idx...> lhs, float rhs) { return lhs *= rhs; }
            friend swizzle<Idx...> operator/(float lhs, swizzle<Idx...> rhs) { return rhs *= lhs; }
            template <size_t...UIdx, typename = std::enable_if_t<(sizeof...(Idx)==sizeof...(UIdx))>>
            friend bool operator==(swizzle<Idx...> lhs, swizzle<UIdx...> rhs) {
                return (Vector<sizeof...(Idx)>)lhs == (Vector<sizeof...(UIdx)>)rhs;
            }
            friend bool operator==(swizzle<Idx...> lhs, Vector<sizeof...(Idx)> rhs) { return (Vector<sizeof...(Idx)>)lhs == rhs; }
            friend bool operator==(Vector<sizeof...(Idx)> lhs, swizzle<Idx...> rhs) { return (Vector<sizeof...(Idx)>)rhs == lhs; }
            template <size_t...UIdx, typename = std::enable_if_t<(sizeof...(Idx)==sizeof...(UIdx))>>
            friend bool operator!=(swizzle<Idx...> lhs, swizzle<UIdx...> rhs) { return !(lhs == rhs); }
            friend bool operator!=(swizzle<Idx...> lhs, Vector<sizeof...(Idx)> rhs) { return !(lhs == rhs); }
            friend bool operator!=(Vector<sizeof...(Idx)> lhs, swizzle<Idx...> rhs) { return !(lhs == rhs); }

            // Size 1 (float) operators.
            template <typename = std::enable_if_t<(sizeof...(Idx)==1)>>
            operator float() const {
                return float(((m_data[Idx]),...));
            }
            template <typename = std::enable_if_t<(sizeof...(Idx)==1)>>
            auto operator=(const float& other) {
                ((m_data[Idx] = other),...);
                return (*this);
            }
            template <typename = std::enable_if_t<(sizeof...(Idx)==1)>>
            auto operator++(int) {
                ((m_data[Idx]++),...);
                return (*this);
            }
            template <typename = std::enable_if_t<(sizeof...(Idx)==1)>>
            auto operator++() {
                ((++m_data[Idx]),...);
                return (*this);
            }
            template <typename = std::enable_if_t<(sizeof...(Idx)==1)>>
            auto operator--(int) {
                ((m_data[Idx]---),...);
                return (*this);
            }
            template <typename = std::enable_if_t<(sizeof...(Idx)==1)>>
            auto operator--() {
                ((--m_data[Idx]),...);
                return (*this);
            }
            template <typename = std::enable_if_t<(sizeof...(Idx)==1)>>
            auto operator+=(const float& other) {
                ((m_data[Idx] += other),...);
                return (*this);
            }
            template <typename = std::enable_if_t<(sizeof...(Idx)==1)>>
            auto operator-=(const float& other) {
                ((m_data[Idx] -= other),...);
                return (*this);
            }
        private:
            float m_data[N];
            template<size_t...> friend class swizzle;
        };

        union {
            float data[N];
            std::conditional_t<(N==2 || N==3 || N==4), swizzle<0>, Empty> x, r;
            std::conditional_t<(N==2 || N==3 || N==4), swizzle<1>, Empty> y, g;
            std::conditional_t<(N==3 || N==4), swizzle<2>, Empty> z, b;
            std::conditional_t<(N==4), swizzle<3>, Empty> w, a;
            std::conditional_t<(N==2 || N==3 || N==4), swizzle<0, 0>, Empty> xx, rr;
            std::conditional_t<(N==2 || N==3 || N==4), swizzle<0, 1>, Empty> xy, rg;
            std::conditional_t<(N==3 || N==4), swizzle<0, 2>, Empty> xz, rb;
            std::conditional_t<(N==4), swizzle<0, 3>, Empty> xw, ra;
            std::conditional_t<(N==2 || N==3 || N==4), swizzle<1, 0>, Empty> yx, gr;
            std::conditional_t<(N==2 || N==3 || N==4), swizzle<1, 1>, Empty> yy, gg;
            std::conditional_t<(N==3 || N==4), swizzle<1, 2>, Empty> yz, gb;
            std::conditional_t<(N==4), swizzle<1, 3>, Empty> yw, ga;
            std::conditional_t<(N==3 || N==4), swizzle<2, 0>, Empty> zx, br;
            std::conditional_t<(N==3 || N==4), swizzle<2, 1>, Empty> zy, bg;
            std::conditional_t<(N==3 || N==4), swizzle<2, 2>, Empty> zz, bb;
            std::conditional_t<(N==4), swizzle<2, 3>, Empty> zw, ba;
            std::conditional_t<(N==4), swizzle<3, 0>, Empty> wx, ar;
            std::conditional_t<(N==4), swizzle<3, 1>, Empty> wy, ag;
            std::conditional_t<(N==4), swizzle<3, 2>, Empty> wz, ab;
            std::conditional_t<(N==4), swizzle<3, 3>, Empty> ww, aa;
            std::conditional_t<(N==3 || N==4), swizzle<0, 0, 0>, Empty> xxx, rrr;
            std::conditional_t<(N==3 || N==4), swizzle<0, 0, 1>, Empty> xxy, rrg;
            std::conditional_t<(N==3 || N==4), swizzle<0, 0, 2>, Empty> xxz, rrb;
            std::conditional_t<(N==4), swizzle<0, 0, 3>, Empty> xxw, rra;
            std::conditional_t<(N==3 || N==4), swizzle<0, 1, 0>, Empty> xyx, rgr;
            std::conditional_t<(N==3 || N==4), swizzle<0, 1, 1>, Empty> xyy, rgg;
            std::conditional_t<(N==3 || N==4), swizzle<0, 1, 2>, Empty> xyz, rgb;
            std::conditional_t<(N==4), swizzle<0, 1, 3>, Empty> xyw, rga;
            std::conditional_t<(N==3 || N==4), swizzle<0, 2, 0>, Empty> xzx, rbr;
            std::conditional_t<(N==3 || N==4), swizzle<0, 2, 1>, Empty> xzy, rbg;
            std::conditional_t<(N==3 || N==4), swizzle<0, 2, 2>, Empty> xzz, rbb;
            std::conditional_t<(N==4), swizzle<0, 2, 3>, Empty> xzw, rba;
            std::conditional_t<(N==4), swizzle<0, 3, 0>, Empty> xwx, rar;
            std::conditional_t<(N==4), swizzle<0, 3, 1>, Empty> xwy, rag;
            std::conditional_t<(N==4), swizzle<0, 3, 2>, Empty> xwz, rab;
            std::conditional_t<(N==4), swizzle<0, 3, 3>, Empty> xww, raa;
            std::conditional_t<(N==3 || N==4), swizzle<1, 0, 0>, Empty> yxx, grr;
            std::conditional_t<(N==3 || N==4), swizzle<1, 0, 1>, Empty> yxy, grg;
            std::conditional_t<(N==3 || N==4), swizzle<1, 0, 2>, Empty> yxz, grb;
            std::conditional_t<(N==4), swizzle<1, 0, 3>, Empty> yxw, gra;
            std::conditional_t<(N==3 || N==4), swizzle<1, 1, 0>, Empty> yyx, ggr;
            std::conditional_t<(N==3 || N==4), swizzle<1, 1, 1>, Empty> yyy, ggg;
            std::conditional_t<(N==3 || N==4), swizzle<1, 1, 2>, Empty> yyz, ggb;
            std::conditional_t<(N==4), swizzle<1, 1, 3>, Empty> yyw, gga;
            std::conditional_t<(N==3 || N==4), swizzle<1, 2, 0>, Empty> yzx, gbr;
            std::conditional_t<(N==3 || N==4), swizzle<1, 2, 1>, Empty> yzy, gbg;
            std::conditional_t<(N==3 || N==4), swizzle<1, 2, 2>, Empty> yzz, gbb;
            std::conditional_t<(N==4), swizzle<1, 2, 3>, Empty> yzw, gba;
            std::conditional_t<(N==4), swizzle<1, 3, 0>, Empty> ywx, gar;
            std::conditional_t<(N==4), swizzle<1, 3, 1>, Empty> ywy, gag;
            std::conditional_t<(N==4), swizzle<1, 3, 2>, Empty> ywz, gab;
            std::conditional_t<(N==4), swizzle<1, 3, 3>, Empty> yww, gaa;
            std::conditional_t<(N==3 || N==4), swizzle<2, 0, 0>, Empty> zxx, brr;
            std::conditional_t<(N==3 || N==4), swizzle<2, 0, 1>, Empty> zxy, brg;
            std::conditional_t<(N==3 || N==4), swizzle<2, 0, 2>, Empty> zxz, brb;
            std::conditional_t<(N==4), swizzle<2, 0, 3>, Empty> zxw, bra;
            std::conditional_t<(N==3 || N==4), swizzle<2, 1, 0>, Empty> zyx, bgr;
            std::conditional_t<(N==3 || N==4), swizzle<2, 1, 1>, Empty> zyy, bgg;
            std::conditional_t<(N==3 || N==4), swizzle<2, 1, 2>, Empty> zyz, bgb;
            std::conditional_t<(N==4), swizzle<2, 1, 3>, Empty> zyw, bga;
            std::conditional_t<(N==3 || N==4), swizzle<2, 2, 0>, Empty> zzx, bbr;
            std::conditional_t<(N==3 || N==4), swizzle<2, 2, 1>, Empty> zzy, bbg;
            std::conditional_t<(N==3 || N==4), swizzle<2, 2, 2>, Empty> zzz, bbb;
            std::conditional_t<(N==4), swizzle<2, 2, 3>, Empty> zzw, bba;
            std::conditional_t<(N==4), swizzle<2, 3, 0>, Empty> zwx, bar;
            std::conditional_t<(N==4), swizzle<2, 3, 1>, Empty> zwy, bag;
            std::conditional_t<(N==4), swizzle<2, 3, 2>, Empty> zwz, bab;
            std::conditional_t<(N==4), swizzle<2, 3, 3>, Empty> zww, baa;
            std::conditional_t<(N==4), swizzle<3, 0, 0>, Empty> wxx, arr;
            std::conditional_t<(N==4), swizzle<3, 0, 1>, Empty> wxy, arg;
            std::conditional_t<(N==4), swizzle<3, 0, 2>, Empty> wxz, arb;
            std::conditional_t<(N==4), swizzle<3, 0, 3>, Empty> wxw, ara;
            std::conditional_t<(N==4), swizzle<3, 1, 0>, Empty> wyx, agr;
            std::conditional_t<(N==4), swizzle<3, 1, 1>, Empty> wyy, agg;
            std::conditional_t<(N==4), swizzle<3, 1, 2>, Empty> wyz, agb;
            std::conditional_t<(N==4), swizzle<3, 1, 3>, Empty> wyw, aga;
            std::conditional_t<(N==4), swizzle<3, 2, 0>, Empty> wzx, abr;
            std::conditional_t<(N==4), swizzle<3, 2, 1>, Empty> wzy, abg;
            std::conditional_t<(N==4), swizzle<3, 2, 2>, Empty> wzz, abb;
            std::conditional_t<(N==4), swizzle<3, 2, 3>, Empty> wzw, aba;
            std::conditional_t<(N==4), swizzle<3, 3, 0>, Empty> wwx, aar;
            std::conditional_t<(N==4), swizzle<3, 3, 1>, Empty> wwy, aag;
            std::conditional_t<(N==4), swizzle<3, 3, 2>, Empty> wwz, aab;
            std::conditional_t<(N==4), swizzle<3, 3, 3>, Empty> www, aaa;
            std::conditional_t<(N==4), swizzle<0, 0, 0, 0>, Empty> xxxx, rrrr;
            std::conditional_t<(N==4), swizzle<0, 0, 0, 1>, Empty> xxxy, rrrg;
            std::conditional_t<(N==4), swizzle<0, 0, 0, 2>, Empty> xxxz, rrrb;
            std::conditional_t<(N==4), swizzle<0, 0, 0, 3>, Empty> xxxw, rrra;
            std::conditional_t<(N==4), swizzle<0, 0, 1, 0>, Empty> xxyx, rrgr;
            std::conditional_t<(N==4), swizzle<0, 0, 1, 1>, Empty> xxyy, rrgg;
            std::conditional_t<(N==4), swizzle<0, 0, 1, 2>, Empty> xxyz, rrgb;
            std::conditional_t<(N==4), swizzle<0, 0, 1, 3>, Empty> xxyw, rrga;
            std::conditional_t<(N==4), swizzle<0, 0, 2, 0>, Empty> xxzx, rrbr;
            std::conditional_t<(N==4), swizzle<0, 0, 2, 1>, Empty> xxzy, rrbg;
            std::conditional_t<(N==4), swizzle<0, 0, 2, 2>, Empty> xxzz, rrbb;
            std::conditional_t<(N==4), swizzle<0, 0, 2, 3>, Empty> xxzw, rrba;
            std::conditional_t<(N==4), swizzle<0, 0, 3, 0>, Empty> xxwx, rrar;
            std::conditional_t<(N==4), swizzle<0, 0, 3, 1>, Empty> xxwy, rrag;
            std::conditional_t<(N==4), swizzle<0, 0, 3, 2>, Empty> xxwz, rrab;
            std::conditional_t<(N==4), swizzle<0, 0, 3, 3>, Empty> xxww, rraa;
            std::conditional_t<(N==4), swizzle<0, 1, 0, 0>, Empty> xyxx, rgrr;
            std::conditional_t<(N==4), swizzle<0, 1, 0, 1>, Empty> xyxy, rgrg;
            std::conditional_t<(N==4), swizzle<0, 1, 0, 2>, Empty> xyxz, rgrb;
            std::conditional_t<(N==4), swizzle<0, 1, 0, 3>, Empty> xyxw, rgra;
            std::conditional_t<(N==4), swizzle<0, 1, 1, 0>, Empty> xyyx, rggr;
            std::conditional_t<(N==4), swizzle<0, 1, 1, 1>, Empty> xyyy, rggg;
            std::conditional_t<(N==4), swizzle<0, 1, 1, 2>, Empty> xyyz, rggb;
            std::conditional_t<(N==4), swizzle<0, 1, 1, 3>, Empty> xyyw, rgga;
            std::conditional_t<(N==4), swizzle<0, 1, 2, 0>, Empty> xyzx, rgbr;
            std::conditional_t<(N==4), swizzle<0, 1, 2, 1>, Empty> xyzy, rgbg;
            std::conditional_t<(N==4), swizzle<0, 1, 2, 2>, Empty> xyzz, rgbb;
            std::conditional_t<(N==4), swizzle<0, 1, 2, 3>, Empty> xyzw, rgba;
            std::conditional_t<(N==4), swizzle<0, 1, 3, 0>, Empty> xywx, rgar;
            std::conditional_t<(N==4), swizzle<0, 1, 3, 1>, Empty> xywy, rgag;
            std::conditional_t<(N==4), swizzle<0, 1, 3, 2>, Empty> xywz, rgab;
            std::conditional_t<(N==4), swizzle<0, 1, 3, 3>, Empty> xyww, rgaa;
            std::conditional_t<(N==4), swizzle<0, 2, 0, 0>, Empty> xzxx, rbrr;
            std::conditional_t<(N==4), swizzle<0, 2, 0, 1>, Empty> xzxy, rbrg;
            std::conditional_t<(N==4), swizzle<0, 2, 0, 2>, Empty> xzxz, rbrb;
            std::conditional_t<(N==4), swizzle<0, 2, 0, 3>, Empty> xzxw, rbra;
            std::conditional_t<(N==4), swizzle<0, 2, 1, 0>, Empty> xzyx, rbgr;
            std::conditional_t<(N==4), swizzle<0, 2, 1, 1>, Empty> xzyy, rbgg;
            std::conditional_t<(N==4), swizzle<0, 2, 1, 2>, Empty> xzyz, rbgb;
            std::conditional_t<(N==4), swizzle<0, 2, 1, 3>, Empty> xzyw, rbga;
            std::conditional_t<(N==4), swizzle<0, 2, 2, 0>, Empty> xzzx, rbbr;
            std::conditional_t<(N==4), swizzle<0, 2, 2, 1>, Empty> xzzy, rbbg;
            std::conditional_t<(N==4), swizzle<0, 2, 2, 2>, Empty> xzzz, rbbb;
            std::conditional_t<(N==4), swizzle<0, 2, 2, 3>, Empty> xzzw, rbba;
            std::conditional_t<(N==4), swizzle<0, 2, 3, 0>, Empty> xzwx, rbar;
            std::conditional_t<(N==4), swizzle<0, 2, 3, 1>, Empty> xzwy, rbag;
            std::conditional_t<(N==4), swizzle<0, 2, 3, 2>, Empty> xzwz, rbab;
            std::conditional_t<(N==4), swizzle<0, 2, 3, 3>, Empty> xzww, rbaa;
            std::conditional_t<(N==4), swizzle<0, 3, 0, 0>, Empty> xwxx, rarr;
            std::conditional_t<(N==4), swizzle<0, 3, 0, 1>, Empty> xwxy, rarg;
            std::conditional_t<(N==4), swizzle<0, 3, 0, 2>, Empty> xwxz, rarb;
            std::conditional_t<(N==4), swizzle<0, 3, 0, 3>, Empty> xwxw, rara;
            std::conditional_t<(N==4), swizzle<0, 3, 1, 0>, Empty> xwyx, ragr;
            std::conditional_t<(N==4), swizzle<0, 3, 1, 1>, Empty> xwyy, ragg;
            std::conditional_t<(N==4), swizzle<0, 3, 1, 2>, Empty> xwyz, ragb;
            std::conditional_t<(N==4), swizzle<0, 3, 1, 3>, Empty> xwyw, raga;
            std::conditional_t<(N==4), swizzle<0, 3, 2, 0>, Empty> xwzx, rabr;
            std::conditional_t<(N==4), swizzle<0, 3, 2, 1>, Empty> xwzy, rabg;
            std::conditional_t<(N==4), swizzle<0, 3, 2, 2>, Empty> xwzz, rabb;
            std::conditional_t<(N==4), swizzle<0, 3, 2, 3>, Empty> xwzw, raba;
            std::conditional_t<(N==4), swizzle<0, 3, 3, 0>, Empty> xwwx, raar;
            std::conditional_t<(N==4), swizzle<0, 3, 3, 1>, Empty> xwwy, raag;
            std::conditional_t<(N==4), swizzle<0, 3, 3, 2>, Empty> xwwz, raab;
            std::conditional_t<(N==4), swizzle<0, 3, 3, 3>, Empty> xwww, raaa;
            std::conditional_t<(N==4), swizzle<1, 0, 0, 0>, Empty> yxxx, grrr;
            std::conditional_t<(N==4), swizzle<1, 0, 0, 1>, Empty> yxxy, grrg;
            std::conditional_t<(N==4), swizzle<1, 0, 0, 2>, Empty> yxxz, grrb;
            std::conditional_t<(N==4), swizzle<1, 0, 0, 3>, Empty> yxxw, grra;
            std::conditional_t<(N==4), swizzle<1, 0, 1, 0>, Empty> yxyx, grgr;
            std::conditional_t<(N==4), swizzle<1, 0, 1, 1>, Empty> yxyy, grgg;
            std::conditional_t<(N==4), swizzle<1, 0, 1, 2>, Empty> yxyz, grgb;
            std::conditional_t<(N==4), swizzle<1, 0, 1, 3>, Empty> yxyw, grga;
            std::conditional_t<(N==4), swizzle<1, 0, 2, 0>, Empty> yxzx, grbr;
            std::conditional_t<(N==4), swizzle<1, 0, 2, 1>, Empty> yxzy, grbg;
            std::conditional_t<(N==4), swizzle<1, 0, 2, 2>, Empty> yxzz, grbb;
            std::conditional_t<(N==4), swizzle<1, 0, 2, 3>, Empty> yxzw, grba;
            std::conditional_t<(N==4), swizzle<1, 0, 3, 0>, Empty> yxwx, grar;
            std::conditional_t<(N==4), swizzle<1, 0, 3, 1>, Empty> yxwy, grag;
            std::conditional_t<(N==4), swizzle<1, 0, 3, 2>, Empty> yxwz, grab;
            std::conditional_t<(N==4), swizzle<1, 0, 3, 3>, Empty> yxww, graa;
            std::conditional_t<(N==4), swizzle<1, 1, 0, 0>, Empty> yyxx, ggrr;
            std::conditional_t<(N==4), swizzle<1, 1, 0, 1>, Empty> yyxy, ggrg;
            std::conditional_t<(N==4), swizzle<1, 1, 0, 2>, Empty> yyxz, ggrb;
            std::conditional_t<(N==4), swizzle<1, 1, 0, 3>, Empty> yyxw, ggra;
            std::conditional_t<(N==4), swizzle<1, 1, 1, 0>, Empty> yyyx, gggr;
            std::conditional_t<(N==4), swizzle<1, 1, 1, 1>, Empty> yyyy, gggg;
            std::conditional_t<(N==4), swizzle<1, 1, 1, 2>, Empty> yyyz, gggb;
            std::conditional_t<(N==4), swizzle<1, 1, 1, 3>, Empty> yyyw, ggga;
            std::conditional_t<(N==4), swizzle<1, 1, 2, 0>, Empty> yyzx, ggbr;
            std::conditional_t<(N==4), swizzle<1, 1, 2, 1>, Empty> yyzy, ggbg;
            std::conditional_t<(N==4), swizzle<1, 1, 2, 2>, Empty> yyzz, ggbb;
            std::conditional_t<(N==4), swizzle<1, 1, 2, 3>, Empty> yyzw, ggba;
            std::conditional_t<(N==4), swizzle<1, 1, 3, 0>, Empty> yywx, ggar;
            std::conditional_t<(N==4), swizzle<1, 1, 3, 1>, Empty> yywy, ggag;
            std::conditional_t<(N==4), swizzle<1, 1, 3, 2>, Empty> yywz, ggab;
            std::conditional_t<(N==4), swizzle<1, 1, 3, 3>, Empty> yyww, ggaa;
            std::conditional_t<(N==4), swizzle<1, 2, 0, 0>, Empty> yzxx, gbrr;
            std::conditional_t<(N==4), swizzle<1, 2, 0, 1>, Empty> yzxy, gbrg;
            std::conditional_t<(N==4), swizzle<1, 2, 0, 2>, Empty> yzxz, gbrb;
            std::conditional_t<(N==4), swizzle<1, 2, 0, 3>, Empty> yzxw, gbra;
            std::conditional_t<(N==4), swizzle<1, 2, 1, 0>, Empty> yzyx, gbgr;
            std::conditional_t<(N==4), swizzle<1, 2, 1, 1>, Empty> yzyy, gbgg;
            std::conditional_t<(N==4), swizzle<1, 2, 1, 2>, Empty> yzyz, gbgb;
            std::conditional_t<(N==4), swizzle<1, 2, 1, 3>, Empty> yzyw, gbga;
            std::conditional_t<(N==4), swizzle<1, 2, 2, 0>, Empty> yzzx, gbbr;
            std::conditional_t<(N==4), swizzle<1, 2, 2, 1>, Empty> yzzy, gbbg;
            std::conditional_t<(N==4), swizzle<1, 2, 2, 2>, Empty> yzzz, gbbb;
            std::conditional_t<(N==4), swizzle<1, 2, 2, 3>, Empty> yzzw, gbba;
            std::conditional_t<(N==4), swizzle<1, 2, 3, 0>, Empty> yzwx, gbar;
            std::conditional_t<(N==4), swizzle<1, 2, 3, 1>, Empty> yzwy, gbag;
            std::conditional_t<(N==4), swizzle<1, 2, 3, 2>, Empty> yzwz, gbab;
            std::conditional_t<(N==4), swizzle<1, 2, 3, 3>, Empty> yzww, gbaa;
            std::conditional_t<(N==4), swizzle<1, 3, 0, 0>, Empty> ywxx, garr;
            std::conditional_t<(N==4), swizzle<1, 3, 0, 1>, Empty> ywxy, garg;
            std::conditional_t<(N==4), swizzle<1, 3, 0, 2>, Empty> ywxz, garb;
            std::conditional_t<(N==4), swizzle<1, 3, 0, 3>, Empty> ywxw, gara;
            std::conditional_t<(N==4), swizzle<1, 3, 1, 0>, Empty> ywyx, gagr;
            std::conditional_t<(N==4), swizzle<1, 3, 1, 1>, Empty> ywyy, gagg;
            std::conditional_t<(N==4), swizzle<1, 3, 1, 2>, Empty> ywyz, gagb;
            std::conditional_t<(N==4), swizzle<1, 3, 1, 3>, Empty> ywyw, gaga;
            std::conditional_t<(N==4), swizzle<1, 3, 2, 0>, Empty> ywzx, gabr;
            std::conditional_t<(N==4), swizzle<1, 3, 2, 1>, Empty> ywzy, gabg;
            std::conditional_t<(N==4), swizzle<1, 3, 2, 2>, Empty> ywzz, gabb;
            std::conditional_t<(N==4), swizzle<1, 3, 2, 3>, Empty> ywzw, gaba;
            std::conditional_t<(N==4), swizzle<1, 3, 3, 0>, Empty> ywwx, gaar;
            std::conditional_t<(N==4), swizzle<1, 3, 3, 1>, Empty> ywwy, gaag;
            std::conditional_t<(N==4), swizzle<1, 3, 3, 2>, Empty> ywwz, gaab;
            std::conditional_t<(N==4), swizzle<1, 3, 3, 3>, Empty> ywww, gaaa;
            std::conditional_t<(N==4), swizzle<2, 0, 0, 0>, Empty> zxxx, brrr;
            std::conditional_t<(N==4), swizzle<2, 0, 0, 1>, Empty> zxxy, brrg;
            std::conditional_t<(N==4), swizzle<2, 0, 0, 2>, Empty> zxxz, brrb;
            std::conditional_t<(N==4), swizzle<2, 0, 0, 3>, Empty> zxxw, brra;
            std::conditional_t<(N==4), swizzle<2, 0, 1, 0>, Empty> zxyx, brgr;
            std::conditional_t<(N==4), swizzle<2, 0, 1, 1>, Empty> zxyy, brgg;
            std::conditional_t<(N==4), swizzle<2, 0, 1, 2>, Empty> zxyz, brgb;
            std::conditional_t<(N==4), swizzle<2, 0, 1, 3>, Empty> zxyw, brga;
            std::conditional_t<(N==4), swizzle<2, 0, 2, 0>, Empty> zxzx, brbr;
            std::conditional_t<(N==4), swizzle<2, 0, 2, 1>, Empty> zxzy, brbg;
            std::conditional_t<(N==4), swizzle<2, 0, 2, 2>, Empty> zxzz, brbb;
            std::conditional_t<(N==4), swizzle<2, 0, 2, 3>, Empty> zxzw, brba;
            std::conditional_t<(N==4), swizzle<2, 0, 3, 0>, Empty> zxwx, brar;
            std::conditional_t<(N==4), swizzle<2, 0, 3, 1>, Empty> zxwy, brag;
            std::conditional_t<(N==4), swizzle<2, 0, 3, 2>, Empty> zxwz, brab;
            std::conditional_t<(N==4), swizzle<2, 0, 3, 3>, Empty> zxww, braa;
            std::conditional_t<(N==4), swizzle<2, 1, 0, 0>, Empty> zyxx, bgrr;
            std::conditional_t<(N==4), swizzle<2, 1, 0, 1>, Empty> zyxy, bgrg;
            std::conditional_t<(N==4), swizzle<2, 1, 0, 2>, Empty> zyxz, bgrb;
            std::conditional_t<(N==4), swizzle<2, 1, 0, 3>, Empty> zyxw, bgra;
            std::conditional_t<(N==4), swizzle<2, 1, 1, 0>, Empty> zyyx, bggr;
            std::conditional_t<(N==4), swizzle<2, 1, 1, 1>, Empty> zyyy, bggg;
            std::conditional_t<(N==4), swizzle<2, 1, 1, 2>, Empty> zyyz, bggb;
            std::conditional_t<(N==4), swizzle<2, 1, 1, 3>, Empty> zyyw, bgga;
            std::conditional_t<(N==4), swizzle<2, 1, 2, 0>, Empty> zyzx, bgbr;
            std::conditional_t<(N==4), swizzle<2, 1, 2, 1>, Empty> zyzy, bgbg;
            std::conditional_t<(N==4), swizzle<2, 1, 2, 2>, Empty> zyzz, bgbb;
            std::conditional_t<(N==4), swizzle<2, 1, 2, 3>, Empty> zyzw, bgba;
            std::conditional_t<(N==4), swizzle<2, 1, 3, 0>, Empty> zywx, bgar;
            std::conditional_t<(N==4), swizzle<2, 1, 3, 1>, Empty> zywy, bgag;
            std::conditional_t<(N==4), swizzle<2, 1, 3, 2>, Empty> zywz, bgab;
            std::conditional_t<(N==4), swizzle<2, 1, 3, 3>, Empty> zyww, bgaa;
            std::conditional_t<(N==4), swizzle<2, 2, 0, 0>, Empty> zzxx, bbrr;
            std::conditional_t<(N==4), swizzle<2, 2, 0, 1>, Empty> zzxy, bbrg;
            std::conditional_t<(N==4), swizzle<2, 2, 0, 2>, Empty> zzxz, bbrb;
            std::conditional_t<(N==4), swizzle<2, 2, 0, 3>, Empty> zzxw, bbra;
            std::conditional_t<(N==4), swizzle<2, 2, 1, 0>, Empty> zzyx, bbgr;
            std::conditional_t<(N==4), swizzle<2, 2, 1, 1>, Empty> zzyy, bbgg;
            std::conditional_t<(N==4), swizzle<2, 2, 1, 2>, Empty> zzyz, bbgb;
            std::conditional_t<(N==4), swizzle<2, 2, 1, 3>, Empty> zzyw, bbga;
            std::conditional_t<(N==4), swizzle<2, 2, 2, 0>, Empty> zzzx, bbbr;
            std::conditional_t<(N==4), swizzle<2, 2, 2, 1>, Empty> zzzy, bbbg;
            std::conditional_t<(N==4), swizzle<2, 2, 2, 2>, Empty> zzzz, bbbb;
            std::conditional_t<(N==4), swizzle<2, 2, 2, 3>, Empty> zzzw, bbba;
            std::conditional_t<(N==4), swizzle<2, 2, 3, 0>, Empty> zzwx, bbar;
            std::conditional_t<(N==4), swizzle<2, 2, 3, 1>, Empty> zzwy, bbag;
            std::conditional_t<(N==4), swizzle<2, 2, 3, 2>, Empty> zzwz, bbab;
            std::conditional_t<(N==4), swizzle<2, 2, 3, 3>, Empty> zzww, bbaa;
            std::conditional_t<(N==4), swizzle<2, 3, 0, 0>, Empty> zwxx, barr;
            std::conditional_t<(N==4), swizzle<2, 3, 0, 1>, Empty> zwxy, barg;
            std::conditional_t<(N==4), swizzle<2, 3, 0, 2>, Empty> zwxz, barb;
            std::conditional_t<(N==4), swizzle<2, 3, 0, 3>, Empty> zwxw, bara;
            std::conditional_t<(N==4), swizzle<2, 3, 1, 0>, Empty> zwyx, bagr;
            std::conditional_t<(N==4), swizzle<2, 3, 1, 1>, Empty> zwyy, bagg;
            std::conditional_t<(N==4), swizzle<2, 3, 1, 2>, Empty> zwyz, bagb;
            std::conditional_t<(N==4), swizzle<2, 3, 1, 3>, Empty> zwyw, baga;
            std::conditional_t<(N==4), swizzle<2, 3, 2, 0>, Empty> zwzx, babr;
            std::conditional_t<(N==4), swizzle<2, 3, 2, 1>, Empty> zwzy, babg;
            std::conditional_t<(N==4), swizzle<2, 3, 2, 2>, Empty> zwzz, babb;
            std::conditional_t<(N==4), swizzle<2, 3, 2, 3>, Empty> zwzw, baba;
            std::conditional_t<(N==4), swizzle<2, 3, 3, 0>, Empty> zwwx, baar;
            std::conditional_t<(N==4), swizzle<2, 3, 3, 1>, Empty> zwwy, baag;
            std::conditional_t<(N==4), swizzle<2, 3, 3, 2>, Empty> zwwz, baab;
            std::conditional_t<(N==4), swizzle<2, 3, 3, 3>, Empty> zwww, baaa;
            std::conditional_t<(N==4), swizzle<3, 0, 0, 0>, Empty> wxxx, arrr;
            std::conditional_t<(N==4), swizzle<3, 0, 0, 1>, Empty> wxxy, arrg;
            std::conditional_t<(N==4), swizzle<3, 0, 0, 2>, Empty> wxxz, arrb;
            std::conditional_t<(N==4), swizzle<3, 0, 0, 3>, Empty> wxxw, arra;
            std::conditional_t<(N==4), swizzle<3, 0, 1, 0>, Empty> wxyx, argr;
            std::conditional_t<(N==4), swizzle<3, 0, 1, 1>, Empty> wxyy, argg;
            std::conditional_t<(N==4), swizzle<3, 0, 1, 2>, Empty> wxyz, argb;
            std::conditional_t<(N==4), swizzle<3, 0, 1, 3>, Empty> wxyw, arga;
            std::conditional_t<(N==4), swizzle<3, 0, 2, 0>, Empty> wxzx, arbr;
            std::conditional_t<(N==4), swizzle<3, 0, 2, 1>, Empty> wxzy, arbg;
            std::conditional_t<(N==4), swizzle<3, 0, 2, 2>, Empty> wxzz, arbb;
            std::conditional_t<(N==4), swizzle<3, 0, 2, 3>, Empty> wxzw, arba;
            std::conditional_t<(N==4), swizzle<3, 0, 3, 0>, Empty> wxwx, arar;
            std::conditional_t<(N==4), swizzle<3, 0, 3, 1>, Empty> wxwy, arag;
            std::conditional_t<(N==4), swizzle<3, 0, 3, 2>, Empty> wxwz, arab;
            std::conditional_t<(N==4), swizzle<3, 0, 3, 3>, Empty> wxww, araa;
            std::conditional_t<(N==4), swizzle<3, 1, 0, 0>, Empty> wyxx, agrr;
            std::conditional_t<(N==4), swizzle<3, 1, 0, 1>, Empty> wyxy, agrg;
            std::conditional_t<(N==4), swizzle<3, 1, 0, 2>, Empty> wyxz, agrb;
            std::conditional_t<(N==4), swizzle<3, 1, 0, 3>, Empty> wyxw, agra;
            std::conditional_t<(N==4), swizzle<3, 1, 1, 0>, Empty> wyyx, aggr;
            std::conditional_t<(N==4), swizzle<3, 1, 1, 1>, Empty> wyyy, aggg;
            std::conditional_t<(N==4), swizzle<3, 1, 1, 2>, Empty> wyyz, aggb;
            std::conditional_t<(N==4), swizzle<3, 1, 1, 3>, Empty> wyyw, agga;
            std::conditional_t<(N==4), swizzle<3, 1, 2, 0>, Empty> wyzx, agbr;
            std::conditional_t<(N==4), swizzle<3, 1, 2, 1>, Empty> wyzy, agbg;
            std::conditional_t<(N==4), swizzle<3, 1, 2, 2>, Empty> wyzz, agbb;
            std::conditional_t<(N==4), swizzle<3, 1, 2, 3>, Empty> wyzw, agba;
            std::conditional_t<(N==4), swizzle<3, 1, 3, 0>, Empty> wywx, agar;
            std::conditional_t<(N==4), swizzle<3, 1, 3, 1>, Empty> wywy, agag;
            std::conditional_t<(N==4), swizzle<3, 1, 3, 2>, Empty> wywz, agab;
            std::conditional_t<(N==4), swizzle<3, 1, 3, 3>, Empty> wyww, agaa;
            std::conditional_t<(N==4), swizzle<3, 2, 0, 0>, Empty> wzxx, abrr;
            std::conditional_t<(N==4), swizzle<3, 2, 0, 1>, Empty> wzxy, abrg;
            std::conditional_t<(N==4), swizzle<3, 2, 0, 2>, Empty> wzxz, abrb;
            std::conditional_t<(N==4), swizzle<3, 2, 0, 3>, Empty> wzxw, abra;
            std::conditional_t<(N==4), swizzle<3, 2, 1, 0>, Empty> wzyx, abgr;
            std::conditional_t<(N==4), swizzle<3, 2, 1, 1>, Empty> wzyy, abgg;
            std::conditional_t<(N==4), swizzle<3, 2, 1, 2>, Empty> wzyz, abgb;
            std::conditional_t<(N==4), swizzle<3, 2, 1, 3>, Empty> wzyw, abga;
            std::conditional_t<(N==4), swizzle<3, 2, 2, 0>, Empty> wzzx, abbr;
            std::conditional_t<(N==4), swizzle<3, 2, 2, 1>, Empty> wzzy, abbg;
            std::conditional_t<(N==4), swizzle<3, 2, 2, 2>, Empty> wzzz, abbb;
            std::conditional_t<(N==4), swizzle<3, 2, 2, 3>, Empty> wzzw, abba;
            std::conditional_t<(N==4), swizzle<3, 2, 3, 0>, Empty> wzwx, abar;
            std::conditional_t<(N==4), swizzle<3, 2, 3, 1>, Empty> wzwy, abag;
            std::conditional_t<(N==4), swizzle<3, 2, 3, 2>, Empty> wzwz, abab;
            std::conditional_t<(N==4), swizzle<3, 2, 3, 3>, Empty> wzww, abaa;
            std::conditional_t<(N==4), swizzle<3, 3, 0, 0>, Empty> wwxx, aarr;
            std::conditional_t<(N==4), swizzle<3, 3, 0, 1>, Empty> wwxy, aarg;
            std::conditional_t<(N==4), swizzle<3, 3, 0, 2>, Empty> wwxz, aarb;
            std::conditional_t<(N==4), swizzle<3, 3, 0, 3>, Empty> wwxw, aara;
            std::conditional_t<(N==4), swizzle<3, 3, 1, 0>, Empty> wwyx, aagr;
            std::conditional_t<(N==4), swizzle<3, 3, 1, 1>, Empty> wwyy, aagg;
            std::conditional_t<(N==4), swizzle<3, 3, 1, 2>, Empty> wwyz, aagb;
            std::conditional_t<(N==4), swizzle<3, 3, 1, 3>, Empty> wwyw, aaga;
            std::conditional_t<(N==4), swizzle<3, 3, 2, 0>, Empty> wwzx, aabr;
            std::conditional_t<(N==4), swizzle<3, 3, 2, 1>, Empty> wwzy, aabg;
            std::conditional_t<(N==4), swizzle<3, 3, 2, 2>, Empty> wwzz, aabb;
            std::conditional_t<(N==4), swizzle<3, 3, 2, 3>, Empty> wwzw, aaba;
            std::conditional_t<(N==4), swizzle<3, 3, 3, 0>, Empty> wwwx, aaar;
            std::conditional_t<(N==4), swizzle<3, 3, 3, 1>, Empty> wwwy, aaag;
            std::conditional_t<(N==4), swizzle<3, 3, 3, 2>, Empty> wwwz, aaab;
            std::conditional_t<(N==4), swizzle<3, 3, 3, 3>, Empty> wwww, aaaa;
        };
    };

    using Vector2 = Vector<2>;
    using Vector3 = Vector<3>;
    using Vector4 = Vector<4>;
}
