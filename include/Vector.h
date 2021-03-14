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
            *this *= (1.0/mag);
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
        template <size_t index>
        class swizzleScalar {
        public:
            swizzleScalar() : ref(NULL) { }
            swizzleScalar(Vector<N> * self) : ref(self) { }
            Vector<N>& GetRef() const {
                return *ref;
            }

            operator float() const {
                return (*ref)[index];
            }
            float& operator=(const float rhs) {
                (*ref)[index] = rhs;
                return (*ref)[index];
            }
            float& operator++(int) {
                (*ref)[index]++;
                return (*ref)[index];
            }
            float& operator++() {
                ++(*ref)[index];
                return (*ref)[index];
            }
            float& operator--(int) {
                (*ref)[index]--;
                return (*ref)[index];
            }
            float& operator--() {
                --(*ref)[index]++;
                return (*ref)[index];
            }
            float& operator+=(const float rhs) {
                (*ref)[index] += rhs;
                return (*ref)[index];
            }
            float& operator-=(const float rhs) {
                (*ref)[index] -= rhs;
                return (*ref)[index];
            }
            float& operator*=(const float rhs) {
                (*ref)[index] *= rhs;
                return (*ref)[index];
            }
            float& operator/=(const float rhs) {
                (*ref)[index] /= rhs;
                return (*ref)[index];
            }
        private:
            Vector<N> * ref;
        };

        template <size_t...Indices>
        class swizzle {
        public:
            swizzle() : ref(NULL) { ExpandIndices(0, Indices...); }
            swizzle(Vector<N> * self) : ref(self) { ExpandIndices(0, Indices...); }
            Vector<N>& GetRef() const {
                return *ref;
            }

            void ExpandIndices(size_t i) {}
            template <typename Arg1, typename...Args> inline void ExpandIndices(size_t i, Arg1 index, const Args... indices) {
                indices[i] = index;
                ExpandIndices(i+1, indices...);
            }

            // Vector operators.
            operator Vector<sizeof...(Indices)>() const {
                return BuildVector();
            }
            float operator [](int i) const { return (*ref)[indices[i]]; }
            float & operator [](int i) { return (*ref)[indices[i]]; }
            Vector<sizeof...(Indices)>& operator=(const Vector<sizeof...(Indices)>& other) {
                for (size_t i = 0; i < sizeof...(Indices); ++i) {
                    (*ref)[indices[i]] = other[i];
                }
                return BuildVector();
            }
            Vector<sizeof...(Indices)>& operator+=(const Vector<sizeof...(Indices)>& other) {
                for (size_t i = 0; i < sizeof...(Indices); ++i) {
                    (*ref)[indices[i]] += other[i];
                }
                return BuildVector();
            }
            Vector<sizeof...(Indices)>& operator+=(const swizzle<Indices...>& other) {
                Vector<sizeof...(Indices)> v = other.BuildVector();
                for (size_t i = 0; i < sizeof...(Indices); ++i) {
                    (*ref)[indices[i]] += v[i];
                }
                return BuildVector();
            }
            friend Vector<sizeof...(Indices)> operator+(swizzle<Indices...> lhs, Vector<sizeof...(Indices)> rhs) { return lhs.BuildVector()+rhs; }
            friend Vector<sizeof...(Indices)> operator+(Vector<sizeof...(Indices)> lhs, swizzle<Indices...> rhs) { return rhs.BuildVector()+lhs; }
            friend Vector<sizeof...(Indices)> operator+(swizzle<Indices...> lhs, swizzle<Indices...> rhs) { return lhs.BuildVector()+rhs.BuildVector(); }
            Vector<sizeof...(Indices)>& operator-=(const Vector<sizeof...(Indices)>& other) {
                for (size_t i = 0; i < sizeof...(Indices); ++i) {
                    (*ref)[indices[i]] += other[i];
                }
                return BuildVector();
            }
            Vector<sizeof...(Indices)>& operator-=(const swizzle<Indices...>& other) {
                Vector<sizeof...(Indices)> v = other.BuildVector();
                for (size_t i = 0; i < size; ++i) {
                    (*ref)[indices[i]] -= v[i];
                }
                return BuildVector();
            }
            friend Vector<sizeof...(Indices)> operator-(swizzle<Indices...> lhs, Vector<sizeof...(Indices)> rhs) { return lhs.BuildVector()-rhs; }
            friend Vector<sizeof...(Indices)> operator-(Vector<sizeof...(Indices)> lhs, swizzle<Indices...> rhs) { return rhs.BuildVector()-lhs; }
            friend Vector<sizeof...(Indices)> operator-(swizzle<Indices...> lhs, swizzle<Indices...> rhs) { return lhs.BuildVector()-rhs.BuildVector(); }
            Vector<sizeof...(Indices)>& operator*=(const float& rhs) {
                for (size_t i=0; i < sizeof...(Indices); ++i) {
                    (*ref)[indices[i]] *= rhs;
                }
                return BuildVector();
            }
            friend Vector<sizeof...(Indices)> operator*(swizzle<Indices...> lhs, const float& rhs) { return lhs.BuildVector() * rhs; }
            friend Vector<sizeof...(Indices)> operator*(const float& lhs, swizzle<Indices...> rhs) { return rhs.BuildVector() * lhs; }
            Vector<sizeof...(Indices)>& operator/=(const float& rhs) {
                for (size_t i=0; i < sizeof...(Indices); ++i) {
                    (*ref)[indices[i]] /= rhs;
                }
                return BuildVector();
            }
            friend Vector<sizeof...(Indices)> operator/(swizzle<Indices...> lhs, const float& rhs) { return lhs.BuildVector() / rhs; }
            friend Vector<sizeof...(Indices)> operator/(const float& lhs, swizzle<Indices...> rhs) { return rhs.BuildVector() / lhs; }
            friend bool operator==(swizzle<Indices...>& lhs, const Vector<sizeof...(Indices)>& rhs) { return lhs.BuildVector() == rhs; }
            friend bool operator==(const Vector<sizeof...(Indices)>& lhs, swizzle<Indices...>& rhs) { return rhs.BuildVector() == lhs; }
            friend bool operator==(swizzle<Indices...>& lhs, swizzle<Indices...>& rhs) { return rhs.BuildVector() == lhs.BuildVector(); }
            friend bool operator!=(swizzle<Indices...>& lhs,swizzle<Indices...>& rhs) { return !(lhs == rhs); }
            friend bool operator!=(swizzle<Indices...>& lhs, const Vector<sizeof...(Indices)>& rhs) { return !(lhs == rhs); }
            friend bool operator!=(const Vector<sizeof...(Indices)>& lhs, swizzle<Indices...>& rhs) { return !(lhs == rhs); }
        private:
            Vector<sizeof...(Indices)> BuildVector() const {
                Vector<sizeof...(Indices)> ret;
                for (size_t i = 0; i < sizeof...(Indices); ++i) {
                    ret[i] = (*ref)[indices[i]];
                }
                return ret;
            }

            size_t indices[sizeof...(Indices)];
            Vector<N> * ref;
        };

    protected:
        float data[N];
    };

    class Vector2 : public Vector<2> {
    public:
        Vector2(float t_val = 0) : Vector<2>(t_val) { SetupSwizzle(); }
        Vector2(const Vector<2>& other) : Vector<2>(other) { SetupSwizzle(); }
        Vector2(float t_data[2]) : Vector<2>(t_data) { SetupSwizzle(); }
        Vector2(std::initializer_list<float> list) : Vector<2>(list) { SetupSwizzle(); }
        Vector2(float t_x, float t_y) {
            this->data[0] = t_x;
            this->data[1] = t_y;
            SetupSwizzle();
        }

        static float Dot(Vector2 a, Vector2 b) { return Vector<2>::Dot(a, b); }

        swizzleScalar<0> x;
        swizzleScalar<1> y;
        swizzle<0, 0> xx;
        swizzle<0, 1> xy;
        swizzle<1, 0> yx;
        swizzle<1, 1> yy;

    protected:
        void SetupSwizzle() {
            x = swizzleScalar<0>(this);
            y = swizzleScalar<1>(this);
            xx = swizzle<0, 0>(this);
            xy = swizzle<0, 1>(this);
            yx = swizzle<1, 0>(this);
            yy = swizzle<1, 1>(this);
        }
    };

    class Vector3 : public Vector<3> {
    public:
        Vector3(float t_val = 0) : Vector<3>(t_val) { SetupSwizzle(); }
        Vector3(const Vector<3>& other) : Vector<3>(other) { SetupSwizzle(); }
        Vector3(float t_data[3]) : Vector<3>(t_data) { SetupSwizzle(); }
        Vector3(std::initializer_list<float> list) : Vector<3>(list) { SetupSwizzle(); }
        Vector3(float t_x, float t_y, float t_z) {
            this->data[0] = t_x;
            this->data[1] = t_y;
            this->data[2] = t_z;
            SetupSwizzle();
        }

        static float Dot(Vector3 a, Vector3 b) { return Vector<3>::Dot(a, b); }
        static Vector3 CrossProduct(Vector3 a, Vector3 b);

        swizzleScalar<0> x;
        swizzleScalar<1> y;
        swizzleScalar<2> z;
        swizzle<0, 0> xx;
        swizzle<0, 1> xy;
        swizzle<0, 2> xz;
        swizzle<1, 0> yx;
        swizzle<1, 1> yy;
        swizzle<1, 2> yz;
        swizzle<2, 0> zx;
        swizzle<2, 1> zy;
        swizzle<2, 2> zz;
        swizzle<0, 0, 0> xxx;
        swizzle<0, 0, 1> xxy;
        swizzle<0, 0, 2> xxz;
        swizzle<0, 1, 0> xyx;
        swizzle<0, 1, 1> xyy;
        swizzle<0, 1, 2> xyz;
        swizzle<0, 2, 0> xzx;
        swizzle<0, 2, 1> xzy;
        swizzle<0, 2, 2> xzz;
        swizzle<1, 0, 0> yxx;
        swizzle<1, 0, 1> yxy;
        swizzle<1, 0, 2> yxz;
        swizzle<1, 1, 0> yyx;
        swizzle<1, 1, 1> yyy;
        swizzle<1, 1, 2> yyz;
        swizzle<1, 2, 0> yzx;
        swizzle<1, 2, 1> yzy;
        swizzle<1, 2, 2> yzz;
        swizzle<2, 0, 0> zxx;
        swizzle<2, 0, 1> zxy;
        swizzle<2, 0, 2> zxz;
        swizzle<2, 1, 0> zyx;
        swizzle<2, 1, 1> zyy;
        swizzle<2, 1, 2> zyz;
        swizzle<2, 2, 0> zzx;
        swizzle<2, 2, 1> zzy;
        swizzle<2, 2, 2> zzz;

    protected:
        void SetupSwizzle() {
            x = swizzleScalar<0>(this);
            y = swizzleScalar<1>(this);
            z = swizzleScalar<2>(this);
            xx = swizzle<0, 0>(this);
            xy = swizzle<0, 1>(this);
            xz = swizzle<0, 2>(this);
            yx = swizzle<1, 0>(this);
            yy = swizzle<1, 1>(this);
            yz = swizzle<1, 2>(this);
            zx = swizzle<2, 0>(this);
            zy = swizzle<2, 1>(this);
            zz = swizzle<2, 2>(this);
            xxx = swizzle<0, 0, 0>(this);
            xxy = swizzle<0, 0, 1>(this);
            xxz = swizzle<0, 0, 2>(this);
            xyx = swizzle<0, 1, 0>(this);
            xyy = swizzle<0, 1, 1>(this);
            xyz = swizzle<0, 1, 2>(this);
            xzx = swizzle<0, 2, 0>(this);
            xzy = swizzle<0, 2, 1>(this);
            xzz = swizzle<0, 2, 2>(this);
            yxx = swizzle<1, 0, 0>(this);
            yxy = swizzle<1, 0, 1>(this);
            yxz = swizzle<1, 0, 2>(this);
            yyx = swizzle<1, 1, 0>(this);
            yyy = swizzle<1, 1, 1>(this);
            yyz = swizzle<1, 1, 2>(this);
            yzx = swizzle<1, 2, 0>(this);
            yzy = swizzle<1, 2, 1>(this);
            yzz = swizzle<1, 2, 2>(this);
            zxx = swizzle<2, 0, 0>(this);
            zxy = swizzle<2, 0, 1>(this);
            zxz = swizzle<2, 0, 2>(this);
            zyx = swizzle<2, 1, 0>(this);
            zyy = swizzle<2, 1, 1>(this);
            zyz = swizzle<2, 1, 2>(this);
            zzx = swizzle<2, 2, 0>(this);
            zzy = swizzle<2, 2, 1>(this);
            zzz = swizzle<2, 2, 2>(this);
        }
    };

    class Vector4 : public Vector<4> {
    public:
        Vector4(float t_val = 0) : Vector<4>(t_val) { SetupSwizzle(); }
        Vector4(const Vector<4>& other) : Vector<4>(other) { SetupSwizzle(); }
        Vector4(float t_data[4]) : Vector<4>(t_data) {}
        Vector4(std::initializer_list<float> list) : Vector<4>(list) { SetupSwizzle(); }
        Vector4(float t_x, float t_y, float t_z, float t_w) {
            this->data[0] = t_x;
            this->data[1] = t_y;
            this->data[2] = t_z;
            this->data[3] = t_w;
            SetupSwizzle();
        }

        static float Dot(Vector4 a, Vector4 b) { return Vector<4>::Dot(a, b); }

        swizzleScalar<0> x;
        swizzleScalar<1> y;
        swizzleScalar<2> z;
        swizzleScalar<3> w;
        swizzle<0, 0> xx;
        swizzle<0, 1> xy;
        swizzle<0, 2> xz;
        swizzle<0, 3> xw;
        swizzle<1, 0> yx;
        swizzle<1, 1> yy;
        swizzle<1, 2> yz;
        swizzle<1, 3> yw;
        swizzle<2, 0> zx;
        swizzle<2, 1> zy;
        swizzle<2, 2> zz;
        swizzle<2, 3> zw;
        swizzle<3, 0> wx;
        swizzle<3, 1> wy;
        swizzle<3, 2> wz;
        swizzle<3, 3> ww;
        swizzle<0, 0, 0> xxx;
        swizzle<0, 0, 1> xxy;
        swizzle<0, 0, 2> xxz;
        swizzle<0, 0, 3> xxw;
        swizzle<0, 1, 0> xyx;
        swizzle<0, 1, 1> xyy;
        swizzle<0, 1, 2> xyz;
        swizzle<0, 1, 3> xyw;
        swizzle<0, 2, 0> xzx;
        swizzle<0, 2, 1> xzy;
        swizzle<0, 2, 2> xzz;
        swizzle<0, 2, 3> xzw;
        swizzle<0, 3, 0> xwx;
        swizzle<0, 3, 1> xwy;
        swizzle<0, 3, 2> xwz;
        swizzle<0, 3, 3> xww;
        swizzle<1, 0, 0> yxx;
        swizzle<1, 0, 1> yxy;
        swizzle<1, 0, 2> yxz;
        swizzle<1, 0, 3> yxw;
        swizzle<1, 1, 0> yyx;
        swizzle<1, 1, 1> yyy;
        swizzle<1, 1, 2> yyz;
        swizzle<1, 1, 3> yyw;
        swizzle<1, 2, 0> yzx;
        swizzle<1, 2, 1> yzy;
        swizzle<1, 2, 2> yzz;
        swizzle<1, 2, 3> yzw;
        swizzle<1, 3, 0> ywx;
        swizzle<1, 3, 1> ywy;
        swizzle<1, 3, 2> ywz;
        swizzle<1, 3, 3> yww;
        swizzle<2, 0, 0> zxx;
        swizzle<2, 0, 1> zxy;
        swizzle<2, 0, 2> zxz;
        swizzle<2, 0, 3> zxw;
        swizzle<2, 1, 0> zyx;
        swizzle<2, 1, 1> zyy;
        swizzle<2, 1, 2> zyz;
        swizzle<2, 1, 3> zyw;
        swizzle<2, 2, 0> zzx;
        swizzle<2, 2, 1> zzy;
        swizzle<2, 2, 2> zzz;
        swizzle<2, 2, 3> zzw;
        swizzle<2, 3, 0> zwx;
        swizzle<2, 3, 1> zwy;
        swizzle<2, 3, 2> zwz;
        swizzle<2, 3, 3> zww;
        swizzle<3, 0, 0> wxx;
        swizzle<3, 0, 1> wxy;
        swizzle<3, 0, 2> wxz;
        swizzle<3, 0, 3> wxw;
        swizzle<3, 1, 0> wyx;
        swizzle<3, 1, 1> wyy;
        swizzle<3, 1, 2> wyz;
        swizzle<3, 1, 3> wyw;
        swizzle<3, 2, 0> wzx;
        swizzle<3, 2, 1> wzy;
        swizzle<3, 2, 2> wzz;
        swizzle<3, 2, 3> wzw;
        swizzle<3, 3, 0> wwx;
        swizzle<3, 3, 1> wwy;
        swizzle<3, 3, 2> wwz;
        swizzle<3, 3, 3> www;
        swizzle<0, 0, 0, 0> xxxx;
        swizzle<0, 0, 0, 1> xxxy;
        swizzle<0, 0, 0, 2> xxxz;
        swizzle<0, 0, 0, 3> xxxw;
        swizzle<0, 0, 1, 0> xxyx;
        swizzle<0, 0, 1, 1> xxyy;
        swizzle<0, 0, 1, 2> xxyz;
        swizzle<0, 0, 1, 3> xxyw;
        swizzle<0, 0, 2, 0> xxzx;
        swizzle<0, 0, 2, 1> xxzy;
        swizzle<0, 0, 2, 2> xxzz;
        swizzle<0, 0, 2, 3> xxzw;
        swizzle<0, 0, 3, 0> xxwx;
        swizzle<0, 0, 3, 1> xxwy;
        swizzle<0, 0, 3, 2> xxwz;
        swizzle<0, 0, 3, 3> xxww;
        swizzle<0, 1, 0, 0> xyxx;
        swizzle<0, 1, 0, 1> xyxy;
        swizzle<0, 1, 0, 2> xyxz;
        swizzle<0, 1, 0, 3> xyxw;
        swizzle<0, 1, 1, 0> xyyx;
        swizzle<0, 1, 1, 1> xyyy;
        swizzle<0, 1, 1, 2> xyyz;
        swizzle<0, 1, 1, 3> xyyw;
        swizzle<0, 1, 2, 0> xyzx;
        swizzle<0, 1, 2, 1> xyzy;
        swizzle<0, 1, 2, 2> xyzz;
        swizzle<0, 1, 2, 3> xyzw;
        swizzle<0, 1, 3, 0> xywx;
        swizzle<0, 1, 3, 1> xywy;
        swizzle<0, 1, 3, 2> xywz;
        swizzle<0, 1, 3, 3> xyww;
        swizzle<0, 2, 0, 0> xzxx;
        swizzle<0, 2, 0, 1> xzxy;
        swizzle<0, 2, 0, 2> xzxz;
        swizzle<0, 2, 0, 3> xzxw;
        swizzle<0, 2, 1, 0> xzyx;
        swizzle<0, 2, 1, 1> xzyy;
        swizzle<0, 2, 1, 2> xzyz;
        swizzle<0, 2, 1, 3> xzyw;
        swizzle<0, 2, 2, 0> xzzx;
        swizzle<0, 2, 2, 1> xzzy;
        swizzle<0, 2, 2, 2> xzzz;
        swizzle<0, 2, 2, 3> xzzw;
        swizzle<0, 2, 3, 0> xzwx;
        swizzle<0, 2, 3, 1> xzwy;
        swizzle<0, 2, 3, 2> xzwz;
        swizzle<0, 2, 3, 3> xzww;
        swizzle<0, 3, 0, 0> xwxx;
        swizzle<0, 3, 0, 1> xwxy;
        swizzle<0, 3, 0, 2> xwxz;
        swizzle<0, 3, 0, 3> xwxw;
        swizzle<0, 3, 1, 0> xwyx;
        swizzle<0, 3, 1, 1> xwyy;
        swizzle<0, 3, 1, 2> xwyz;
        swizzle<0, 3, 1, 3> xwyw;
        swizzle<0, 3, 2, 0> xwzx;
        swizzle<0, 3, 2, 1> xwzy;
        swizzle<0, 3, 2, 2> xwzz;
        swizzle<0, 3, 2, 3> xwzw;
        swizzle<0, 3, 3, 0> xwwx;
        swizzle<0, 3, 3, 1> xwwy;
        swizzle<0, 3, 3, 2> xwwz;
        swizzle<0, 3, 3, 3> xwww;
        swizzle<1, 0, 0, 0> yxxx;
        swizzle<1, 0, 0, 1> yxxy;
        swizzle<1, 0, 0, 2> yxxz;
        swizzle<1, 0, 0, 3> yxxw;
        swizzle<1, 0, 1, 0> yxyx;
        swizzle<1, 0, 1, 1> yxyy;
        swizzle<1, 0, 1, 2> yxyz;
        swizzle<1, 0, 1, 3> yxyw;
        swizzle<1, 0, 2, 0> yxzx;
        swizzle<1, 0, 2, 1> yxzy;
        swizzle<1, 0, 2, 2> yxzz;
        swizzle<1, 0, 2, 3> yxzw;
        swizzle<1, 0, 3, 0> yxwx;
        swizzle<1, 0, 3, 1> yxwy;
        swizzle<1, 0, 3, 2> yxwz;
        swizzle<1, 0, 3, 3> yxww;
        swizzle<1, 1, 0, 0> yyxx;
        swizzle<1, 1, 0, 1> yyxy;
        swizzle<1, 1, 0, 2> yyxz;
        swizzle<1, 1, 0, 3> yyxw;
        swizzle<1, 1, 1, 0> yyyx;
        swizzle<1, 1, 1, 1> yyyy;
        swizzle<1, 1, 1, 2> yyyz;
        swizzle<1, 1, 1, 3> yyyw;
        swizzle<1, 1, 2, 0> yyzx;
        swizzle<1, 1, 2, 1> yyzy;
        swizzle<1, 1, 2, 2> yyzz;
        swizzle<1, 1, 2, 3> yyzw;
        swizzle<1, 1, 3, 0> yywx;
        swizzle<1, 1, 3, 1> yywy;
        swizzle<1, 1, 3, 2> yywz;
        swizzle<1, 1, 3, 3> yyww;
        swizzle<1, 2, 0, 0> yzxx;
        swizzle<1, 2, 0, 1> yzxy;
        swizzle<1, 2, 0, 2> yzxz;
        swizzle<1, 2, 0, 3> yzxw;
        swizzle<1, 2, 1, 0> yzyx;
        swizzle<1, 2, 1, 1> yzyy;
        swizzle<1, 2, 1, 2> yzyz;
        swizzle<1, 2, 1, 3> yzyw;
        swizzle<1, 2, 2, 0> yzzx;
        swizzle<1, 2, 2, 1> yzzy;
        swizzle<1, 2, 2, 2> yzzz;
        swizzle<1, 2, 2, 3> yzzw;
        swizzle<1, 2, 3, 0> yzwx;
        swizzle<1, 2, 3, 1> yzwy;
        swizzle<1, 2, 3, 2> yzwz;
        swizzle<1, 2, 3, 3> yzww;
        swizzle<1, 3, 0, 0> ywxx;
        swizzle<1, 3, 0, 1> ywxy;
        swizzle<1, 3, 0, 2> ywxz;
        swizzle<1, 3, 0, 3> ywxw;
        swizzle<1, 3, 1, 0> ywyx;
        swizzle<1, 3, 1, 1> ywyy;
        swizzle<1, 3, 1, 2> ywyz;
        swizzle<1, 3, 1, 3> ywyw;
        swizzle<1, 3, 2, 0> ywzx;
        swizzle<1, 3, 2, 1> ywzy;
        swizzle<1, 3, 2, 2> ywzz;
        swizzle<1, 3, 2, 3> ywzw;
        swizzle<1, 3, 3, 0> ywwx;
        swizzle<1, 3, 3, 1> ywwy;
        swizzle<1, 3, 3, 2> ywwz;
        swizzle<1, 3, 3, 3> ywww;
        swizzle<2, 0, 0, 0> zxxx;
        swizzle<2, 0, 0, 1> zxxy;
        swizzle<2, 0, 0, 2> zxxz;
        swizzle<2, 0, 0, 3> zxxw;
        swizzle<2, 0, 1, 0> zxyx;
        swizzle<2, 0, 1, 1> zxyy;
        swizzle<2, 0, 1, 2> zxyz;
        swizzle<2, 0, 1, 3> zxyw;
        swizzle<2, 0, 2, 0> zxzx;
        swizzle<2, 0, 2, 1> zxzy;
        swizzle<2, 0, 2, 2> zxzz;
        swizzle<2, 0, 2, 3> zxzw;
        swizzle<2, 0, 3, 0> zxwx;
        swizzle<2, 0, 3, 1> zxwy;
        swizzle<2, 0, 3, 2> zxwz;
        swizzle<2, 0, 3, 3> zxww;
        swizzle<2, 1, 0, 0> zyxx;
        swizzle<2, 1, 0, 1> zyxy;
        swizzle<2, 1, 0, 2> zyxz;
        swizzle<2, 1, 0, 3> zyxw;
        swizzle<2, 1, 1, 0> zyyx;
        swizzle<2, 1, 1, 1> zyyy;
        swizzle<2, 1, 1, 2> zyyz;
        swizzle<2, 1, 1, 3> zyyw;
        swizzle<2, 1, 2, 0> zyzx;
        swizzle<2, 1, 2, 1> zyzy;
        swizzle<2, 1, 2, 2> zyzz;
        swizzle<2, 1, 2, 3> zyzw;
        swizzle<2, 1, 3, 0> zywx;
        swizzle<2, 1, 3, 1> zywy;
        swizzle<2, 1, 3, 2> zywz;
        swizzle<2, 1, 3, 3> zyww;
        swizzle<2, 2, 0, 0> zzxx;
        swizzle<2, 2, 0, 1> zzxy;
        swizzle<2, 2, 0, 2> zzxz;
        swizzle<2, 2, 0, 3> zzxw;
        swizzle<2, 2, 1, 0> zzyx;
        swizzle<2, 2, 1, 1> zzyy;
        swizzle<2, 2, 1, 2> zzyz;
        swizzle<2, 2, 1, 3> zzyw;
        swizzle<2, 2, 2, 0> zzzx;
        swizzle<2, 2, 2, 1> zzzy;
        swizzle<2, 2, 2, 2> zzzz;
        swizzle<2, 2, 2, 3> zzzw;
        swizzle<2, 2, 3, 0> zzwx;
        swizzle<2, 2, 3, 1> zzwy;
        swizzle<2, 2, 3, 2> zzwz;
        swizzle<2, 2, 3, 3> zzww;
        swizzle<2, 3, 0, 0> zwxx;
        swizzle<2, 3, 0, 1> zwxy;
        swizzle<2, 3, 0, 2> zwxz;
        swizzle<2, 3, 0, 3> zwxw;
        swizzle<2, 3, 1, 0> zwyx;
        swizzle<2, 3, 1, 1> zwyy;
        swizzle<2, 3, 1, 2> zwyz;
        swizzle<2, 3, 1, 3> zwyw;
        swizzle<2, 3, 2, 0> zwzx;
        swizzle<2, 3, 2, 1> zwzy;
        swizzle<2, 3, 2, 2> zwzz;
        swizzle<2, 3, 2, 3> zwzw;
        swizzle<2, 3, 3, 0> zwwx;
        swizzle<2, 3, 3, 1> zwwy;
        swizzle<2, 3, 3, 2> zwwz;
        swizzle<2, 3, 3, 3> zwww;
        swizzle<3, 0, 0, 0> wxxx;
        swizzle<3, 0, 0, 1> wxxy;
        swizzle<3, 0, 0, 2> wxxz;
        swizzle<3, 0, 0, 3> wxxw;
        swizzle<3, 0, 1, 0> wxyx;
        swizzle<3, 0, 1, 1> wxyy;
        swizzle<3, 0, 1, 2> wxyz;
        swizzle<3, 0, 1, 3> wxyw;
        swizzle<3, 0, 2, 0> wxzx;
        swizzle<3, 0, 2, 1> wxzy;
        swizzle<3, 0, 2, 2> wxzz;
        swizzle<3, 0, 2, 3> wxzw;
        swizzle<3, 0, 3, 0> wxwx;
        swizzle<3, 0, 3, 1> wxwy;
        swizzle<3, 0, 3, 2> wxwz;
        swizzle<3, 0, 3, 3> wxww;
        swizzle<3, 1, 0, 0> wyxx;
        swizzle<3, 1, 0, 1> wyxy;
        swizzle<3, 1, 0, 2> wyxz;
        swizzle<3, 1, 0, 3> wyxw;
        swizzle<3, 1, 1, 0> wyyx;
        swizzle<3, 1, 1, 1> wyyy;
        swizzle<3, 1, 1, 2> wyyz;
        swizzle<3, 1, 1, 3> wyyw;
        swizzle<3, 1, 2, 0> wyzx;
        swizzle<3, 1, 2, 1> wyzy;
        swizzle<3, 1, 2, 2> wyzz;
        swizzle<3, 1, 2, 3> wyzw;
        swizzle<3, 1, 3, 0> wywx;
        swizzle<3, 1, 3, 1> wywy;
        swizzle<3, 1, 3, 2> wywz;
        swizzle<3, 1, 3, 3> wyww;
        swizzle<3, 2, 0, 0> wzxx;
        swizzle<3, 2, 0, 1> wzxy;
        swizzle<3, 2, 0, 2> wzxz;
        swizzle<3, 2, 0, 3> wzxw;
        swizzle<3, 2, 1, 0> wzyx;
        swizzle<3, 2, 1, 1> wzyy;
        swizzle<3, 2, 1, 2> wzyz;
        swizzle<3, 2, 1, 3> wzyw;
        swizzle<3, 2, 2, 0> wzzx;
        swizzle<3, 2, 2, 1> wzzy;
        swizzle<3, 2, 2, 2> wzzz;
        swizzle<3, 2, 2, 3> wzzw;
        swizzle<3, 2, 3, 0> wzwx;
        swizzle<3, 2, 3, 1> wzwy;
        swizzle<3, 2, 3, 2> wzwz;
        swizzle<3, 2, 3, 3> wzww;
        swizzle<3, 3, 0, 0> wwxx;
        swizzle<3, 3, 0, 1> wwxy;
        swizzle<3, 3, 0, 2> wwxz;
        swizzle<3, 3, 0, 3> wwxw;
        swizzle<3, 3, 1, 0> wwyx;
        swizzle<3, 3, 1, 1> wwyy;
        swizzle<3, 3, 1, 2> wwyz;
        swizzle<3, 3, 1, 3> wwyw;
        swizzle<3, 3, 2, 0> wwzx;
        swizzle<3, 3, 2, 1> wwzy;
        swizzle<3, 3, 2, 2> wwzz;
        swizzle<3, 3, 2, 3> wwzw;
        swizzle<3, 3, 3, 0> wwwx;
        swizzle<3, 3, 3, 1> wwwy;
        swizzle<3, 3, 3, 2> wwwz;
        swizzle<3, 3, 3, 3> wwww;

    protected:
        void SetupSwizzle() {
            x = swizzleScalar<0>(this);
            y = swizzleScalar<1>(this);
            z = swizzleScalar<2>(this);
            w = swizzleScalar<3>(this);
            xx = swizzle<0, 0>(this);
            xy = swizzle<0, 1>(this);
            xz = swizzle<0, 2>(this);
            xw = swizzle<0, 3>(this);
            yx = swizzle<1, 0>(this);
            yy = swizzle<1, 1>(this);
            yz = swizzle<1, 2>(this);
            yw = swizzle<1, 3>(this);
            zx = swizzle<2, 0>(this);
            zy = swizzle<2, 1>(this);
            zz = swizzle<2, 2>(this);
            zw = swizzle<2, 3>(this);
            wx = swizzle<3, 0>(this);
            wy = swizzle<3, 1>(this);
            wz = swizzle<3, 2>(this);
            ww = swizzle<3, 3>(this);
            xxx = swizzle<0, 0, 0>(this);
            xxy = swizzle<0, 0, 1>(this);
            xxz = swizzle<0, 0, 2>(this);
            xxw = swizzle<0, 0, 3>(this);
            xyx = swizzle<0, 1, 0>(this);
            xyy = swizzle<0, 1, 1>(this);
            xyz = swizzle<0, 1, 2>(this);
            xyw = swizzle<0, 1, 3>(this);
            xzx = swizzle<0, 2, 0>(this);
            xzy = swizzle<0, 2, 1>(this);
            xzz = swizzle<0, 2, 2>(this);
            xzw = swizzle<0, 2, 3>(this);
            xwx = swizzle<0, 3, 0>(this);
            xwy = swizzle<0, 3, 1>(this);
            xwz = swizzle<0, 3, 2>(this);
            xww = swizzle<0, 3, 3>(this);
            yxx = swizzle<1, 0, 0>(this);
            yxy = swizzle<1, 0, 1>(this);
            yxz = swizzle<1, 0, 2>(this);
            yxw = swizzle<1, 0, 3>(this);
            yyx = swizzle<1, 1, 0>(this);
            yyy = swizzle<1, 1, 1>(this);
            yyz = swizzle<1, 1, 2>(this);
            yyw = swizzle<1, 1, 3>(this);
            yzx = swizzle<1, 2, 0>(this);
            yzy = swizzle<1, 2, 1>(this);
            yzz = swizzle<1, 2, 2>(this);
            yzw = swizzle<1, 2, 3>(this);
            ywx = swizzle<1, 3, 0>(this);
            ywy = swizzle<1, 3, 1>(this);
            ywz = swizzle<1, 3, 2>(this);
            yww = swizzle<1, 3, 3>(this);
            zxx = swizzle<2, 0, 0>(this);
            zxy = swizzle<2, 0, 1>(this);
            zxz = swizzle<2, 0, 2>(this);
            zxw = swizzle<2, 0, 3>(this);
            zyx = swizzle<2, 1, 0>(this);
            zyy = swizzle<2, 1, 1>(this);
            zyz = swizzle<2, 1, 2>(this);
            zyw = swizzle<2, 1, 3>(this);
            zzx = swizzle<2, 2, 0>(this);
            zzy = swizzle<2, 2, 1>(this);
            zzz = swizzle<2, 2, 2>(this);
            zzw = swizzle<2, 2, 3>(this);
            zwx = swizzle<2, 3, 0>(this);
            zwy = swizzle<2, 3, 1>(this);
            zwz = swizzle<2, 3, 2>(this);
            zww = swizzle<2, 3, 3>(this);
            wxx = swizzle<3, 0, 0>(this);
            wxy = swizzle<3, 0, 1>(this);
            wxz = swizzle<3, 0, 2>(this);
            wxw = swizzle<3, 0, 3>(this);
            wyx = swizzle<3, 1, 0>(this);
            wyy = swizzle<3, 1, 1>(this);
            wyz = swizzle<3, 1, 2>(this);
            wyw = swizzle<3, 1, 3>(this);
            wzx = swizzle<3, 2, 0>(this);
            wzy = swizzle<3, 2, 1>(this);
            wzz = swizzle<3, 2, 2>(this);
            wzw = swizzle<3, 2, 3>(this);
            wwx = swizzle<3, 3, 0>(this);
            wwy = swizzle<3, 3, 1>(this);
            wwz = swizzle<3, 3, 2>(this);
            www = swizzle<3, 3, 3>(this);
            xxxx = swizzle<0, 0, 0, 0>(this);
            xxxy = swizzle<0, 0, 0, 1>(this);
            xxxz = swizzle<0, 0, 0, 2>(this);
            xxxw = swizzle<0, 0, 0, 3>(this);
            xxyx = swizzle<0, 0, 1, 0>(this);
            xxyy = swizzle<0, 0, 1, 1>(this);
            xxyz = swizzle<0, 0, 1, 2>(this);
            xxyw = swizzle<0, 0, 1, 3>(this);
            xxzx = swizzle<0, 0, 2, 0>(this);
            xxzy = swizzle<0, 0, 2, 1>(this);
            xxzz = swizzle<0, 0, 2, 2>(this);
            xxzw = swizzle<0, 0, 2, 3>(this);
            xxwx = swizzle<0, 0, 3, 0>(this);
            xxwy = swizzle<0, 0, 3, 1>(this);
            xxwz = swizzle<0, 0, 3, 2>(this);
            xxww = swizzle<0, 0, 3, 3>(this);
            xyxx = swizzle<0, 1, 0, 0>(this);
            xyxy = swizzle<0, 1, 0, 1>(this);
            xyxz = swizzle<0, 1, 0, 2>(this);
            xyxw = swizzle<0, 1, 0, 3>(this);
            xyyx = swizzle<0, 1, 1, 0>(this);
            xyyy = swizzle<0, 1, 1, 1>(this);
            xyyz = swizzle<0, 1, 1, 2>(this);
            xyyw = swizzle<0, 1, 1, 3>(this);
            xyzx = swizzle<0, 1, 2, 0>(this);
            xyzy = swizzle<0, 1, 2, 1>(this);
            xyzz = swizzle<0, 1, 2, 2>(this);
            xyzw = swizzle<0, 1, 2, 3>(this);
            xywx = swizzle<0, 1, 3, 0>(this);
            xywy = swizzle<0, 1, 3, 1>(this);
            xywz = swizzle<0, 1, 3, 2>(this);
            xyww = swizzle<0, 1, 3, 3>(this);
            xzxx = swizzle<0, 2, 0, 0>(this);
            xzxy = swizzle<0, 2, 0, 1>(this);
            xzxz = swizzle<0, 2, 0, 2>(this);
            xzxw = swizzle<0, 2, 0, 3>(this);
            xzyx = swizzle<0, 2, 1, 0>(this);
            xzyy = swizzle<0, 2, 1, 1>(this);
            xzyz = swizzle<0, 2, 1, 2>(this);
            xzyw = swizzle<0, 2, 1, 3>(this);
            xzzx = swizzle<0, 2, 2, 0>(this);
            xzzy = swizzle<0, 2, 2, 1>(this);
            xzzz = swizzle<0, 2, 2, 2>(this);
            xzzw = swizzle<0, 2, 2, 3>(this);
            xzwx = swizzle<0, 2, 3, 0>(this);
            xzwy = swizzle<0, 2, 3, 1>(this);
            xzwz = swizzle<0, 2, 3, 2>(this);
            xzww = swizzle<0, 2, 3, 3>(this);
            xwxx = swizzle<0, 3, 0, 0>(this);
            xwxy = swizzle<0, 3, 0, 1>(this);
            xwxz = swizzle<0, 3, 0, 2>(this);
            xwxw = swizzle<0, 3, 0, 3>(this);
            xwyx = swizzle<0, 3, 1, 0>(this);
            xwyy = swizzle<0, 3, 1, 1>(this);
            xwyz = swizzle<0, 3, 1, 2>(this);
            xwyw = swizzle<0, 3, 1, 3>(this);
            xwzx = swizzle<0, 3, 2, 0>(this);
            xwzy = swizzle<0, 3, 2, 1>(this);
            xwzz = swizzle<0, 3, 2, 2>(this);
            xwzw = swizzle<0, 3, 2, 3>(this);
            xwwx = swizzle<0, 3, 3, 0>(this);
            xwwy = swizzle<0, 3, 3, 1>(this);
            xwwz = swizzle<0, 3, 3, 2>(this);
            xwww = swizzle<0, 3, 3, 3>(this);
            yxxx = swizzle<1, 0, 0, 0>(this);
            yxxy = swizzle<1, 0, 0, 1>(this);
            yxxz = swizzle<1, 0, 0, 2>(this);
            yxxw = swizzle<1, 0, 0, 3>(this);
            yxyx = swizzle<1, 0, 1, 0>(this);
            yxyy = swizzle<1, 0, 1, 1>(this);
            yxyz = swizzle<1, 0, 1, 2>(this);
            yxyw = swizzle<1, 0, 1, 3>(this);
            yxzx = swizzle<1, 0, 2, 0>(this);
            yxzy = swizzle<1, 0, 2, 1>(this);
            yxzz = swizzle<1, 0, 2, 2>(this);
            yxzw = swizzle<1, 0, 2, 3>(this);
            yxwx = swizzle<1, 0, 3, 0>(this);
            yxwy = swizzle<1, 0, 3, 1>(this);
            yxwz = swizzle<1, 0, 3, 2>(this);
            yxww = swizzle<1, 0, 3, 3>(this);
            yyxx = swizzle<1, 1, 0, 0>(this);
            yyxy = swizzle<1, 1, 0, 1>(this);
            yyxz = swizzle<1, 1, 0, 2>(this);
            yyxw = swizzle<1, 1, 0, 3>(this);
            yyyx = swizzle<1, 1, 1, 0>(this);
            yyyy = swizzle<1, 1, 1, 1>(this);
            yyyz = swizzle<1, 1, 1, 2>(this);
            yyyw = swizzle<1, 1, 1, 3>(this);
            yyzx = swizzle<1, 1, 2, 0>(this);
            yyzy = swizzle<1, 1, 2, 1>(this);
            yyzz = swizzle<1, 1, 2, 2>(this);
            yyzw = swizzle<1, 1, 2, 3>(this);
            yywx = swizzle<1, 1, 3, 0>(this);
            yywy = swizzle<1, 1, 3, 1>(this);
            yywz = swizzle<1, 1, 3, 2>(this);
            yyww = swizzle<1, 1, 3, 3>(this);
            yzxx = swizzle<1, 2, 0, 0>(this);
            yzxy = swizzle<1, 2, 0, 1>(this);
            yzxz = swizzle<1, 2, 0, 2>(this);
            yzxw = swizzle<1, 2, 0, 3>(this);
            yzyx = swizzle<1, 2, 1, 0>(this);
            yzyy = swizzle<1, 2, 1, 1>(this);
            yzyz = swizzle<1, 2, 1, 2>(this);
            yzyw = swizzle<1, 2, 1, 3>(this);
            yzzx = swizzle<1, 2, 2, 0>(this);
            yzzy = swizzle<1, 2, 2, 1>(this);
            yzzz = swizzle<1, 2, 2, 2>(this);
            yzzw = swizzle<1, 2, 2, 3>(this);
            yzwx = swizzle<1, 2, 3, 0>(this);
            yzwy = swizzle<1, 2, 3, 1>(this);
            yzwz = swizzle<1, 2, 3, 2>(this);
            yzww = swizzle<1, 2, 3, 3>(this);
            ywxx = swizzle<1, 3, 0, 0>(this);
            ywxy = swizzle<1, 3, 0, 1>(this);
            ywxz = swizzle<1, 3, 0, 2>(this);
            ywxw = swizzle<1, 3, 0, 3>(this);
            ywyx = swizzle<1, 3, 1, 0>(this);
            ywyy = swizzle<1, 3, 1, 1>(this);
            ywyz = swizzle<1, 3, 1, 2>(this);
            ywyw = swizzle<1, 3, 1, 3>(this);
            ywzx = swizzle<1, 3, 2, 0>(this);
            ywzy = swizzle<1, 3, 2, 1>(this);
            ywzz = swizzle<1, 3, 2, 2>(this);
            ywzw = swizzle<1, 3, 2, 3>(this);
            ywwx = swizzle<1, 3, 3, 0>(this);
            ywwy = swizzle<1, 3, 3, 1>(this);
            ywwz = swizzle<1, 3, 3, 2>(this);
            ywww = swizzle<1, 3, 3, 3>(this);
            zxxx = swizzle<2, 0, 0, 0>(this);
            zxxy = swizzle<2, 0, 0, 1>(this);
            zxxz = swizzle<2, 0, 0, 2>(this);
            zxxw = swizzle<2, 0, 0, 3>(this);
            zxyx = swizzle<2, 0, 1, 0>(this);
            zxyy = swizzle<2, 0, 1, 1>(this);
            zxyz = swizzle<2, 0, 1, 2>(this);
            zxyw = swizzle<2, 0, 1, 3>(this);
            zxzx = swizzle<2, 0, 2, 0>(this);
            zxzy = swizzle<2, 0, 2, 1>(this);
            zxzz = swizzle<2, 0, 2, 2>(this);
            zxzw = swizzle<2, 0, 2, 3>(this);
            zxwx = swizzle<2, 0, 3, 0>(this);
            zxwy = swizzle<2, 0, 3, 1>(this);
            zxwz = swizzle<2, 0, 3, 2>(this);
            zxww = swizzle<2, 0, 3, 3>(this);
            zyxx = swizzle<2, 1, 0, 0>(this);
            zyxy = swizzle<2, 1, 0, 1>(this);
            zyxz = swizzle<2, 1, 0, 2>(this);
            zyxw = swizzle<2, 1, 0, 3>(this);
            zyyx = swizzle<2, 1, 1, 0>(this);
            zyyy = swizzle<2, 1, 1, 1>(this);
            zyyz = swizzle<2, 1, 1, 2>(this);
            zyyw = swizzle<2, 1, 1, 3>(this);
            zyzx = swizzle<2, 1, 2, 0>(this);
            zyzy = swizzle<2, 1, 2, 1>(this);
            zyzz = swizzle<2, 1, 2, 2>(this);
            zyzw = swizzle<2, 1, 2, 3>(this);
            zywx = swizzle<2, 1, 3, 0>(this);
            zywy = swizzle<2, 1, 3, 1>(this);
            zywz = swizzle<2, 1, 3, 2>(this);
            zyww = swizzle<2, 1, 3, 3>(this);
            zzxx = swizzle<2, 2, 0, 0>(this);
            zzxy = swizzle<2, 2, 0, 1>(this);
            zzxz = swizzle<2, 2, 0, 2>(this);
            zzxw = swizzle<2, 2, 0, 3>(this);
            zzyx = swizzle<2, 2, 1, 0>(this);
            zzyy = swizzle<2, 2, 1, 1>(this);
            zzyz = swizzle<2, 2, 1, 2>(this);
            zzyw = swizzle<2, 2, 1, 3>(this);
            zzzx = swizzle<2, 2, 2, 0>(this);
            zzzy = swizzle<2, 2, 2, 1>(this);
            zzzz = swizzle<2, 2, 2, 2>(this);
            zzzw = swizzle<2, 2, 2, 3>(this);
            zzwx = swizzle<2, 2, 3, 0>(this);
            zzwy = swizzle<2, 2, 3, 1>(this);
            zzwz = swizzle<2, 2, 3, 2>(this);
            zzww = swizzle<2, 2, 3, 3>(this);
            zwxx = swizzle<2, 3, 0, 0>(this);
            zwxy = swizzle<2, 3, 0, 1>(this);
            zwxz = swizzle<2, 3, 0, 2>(this);
            zwxw = swizzle<2, 3, 0, 3>(this);
            zwyx = swizzle<2, 3, 1, 0>(this);
            zwyy = swizzle<2, 3, 1, 1>(this);
            zwyz = swizzle<2, 3, 1, 2>(this);
            zwyw = swizzle<2, 3, 1, 3>(this);
            zwzx = swizzle<2, 3, 2, 0>(this);
            zwzy = swizzle<2, 3, 2, 1>(this);
            zwzz = swizzle<2, 3, 2, 2>(this);
            zwzw = swizzle<2, 3, 2, 3>(this);
            zwwx = swizzle<2, 3, 3, 0>(this);
            zwwy = swizzle<2, 3, 3, 1>(this);
            zwwz = swizzle<2, 3, 3, 2>(this);
            zwww = swizzle<2, 3, 3, 3>(this);
            wxxx = swizzle<3, 0, 0, 0>(this);
            wxxy = swizzle<3, 0, 0, 1>(this);
            wxxz = swizzle<3, 0, 0, 2>(this);
            wxxw = swizzle<3, 0, 0, 3>(this);
            wxyx = swizzle<3, 0, 1, 0>(this);
            wxyy = swizzle<3, 0, 1, 1>(this);
            wxyz = swizzle<3, 0, 1, 2>(this);
            wxyw = swizzle<3, 0, 1, 3>(this);
            wxzx = swizzle<3, 0, 2, 0>(this);
            wxzy = swizzle<3, 0, 2, 1>(this);
            wxzz = swizzle<3, 0, 2, 2>(this);
            wxzw = swizzle<3, 0, 2, 3>(this);
            wxwx = swizzle<3, 0, 3, 0>(this);
            wxwy = swizzle<3, 0, 3, 1>(this);
            wxwz = swizzle<3, 0, 3, 2>(this);
            wxww = swizzle<3, 0, 3, 3>(this);
            wyxx = swizzle<3, 1, 0, 0>(this);
            wyxy = swizzle<3, 1, 0, 1>(this);
            wyxz = swizzle<3, 1, 0, 2>(this);
            wyxw = swizzle<3, 1, 0, 3>(this);
            wyyx = swizzle<3, 1, 1, 0>(this);
            wyyy = swizzle<3, 1, 1, 1>(this);
            wyyz = swizzle<3, 1, 1, 2>(this);
            wyyw = swizzle<3, 1, 1, 3>(this);
            wyzx = swizzle<3, 1, 2, 0>(this);
            wyzy = swizzle<3, 1, 2, 1>(this);
            wyzz = swizzle<3, 1, 2, 2>(this);
            wyzw = swizzle<3, 1, 2, 3>(this);
            wywx = swizzle<3, 1, 3, 0>(this);
            wywy = swizzle<3, 1, 3, 1>(this);
            wywz = swizzle<3, 1, 3, 2>(this);
            wyww = swizzle<3, 1, 3, 3>(this);
            wzxx = swizzle<3, 2, 0, 0>(this);
            wzxy = swizzle<3, 2, 0, 1>(this);
            wzxz = swizzle<3, 2, 0, 2>(this);
            wzxw = swizzle<3, 2, 0, 3>(this);
            wzyx = swizzle<3, 2, 1, 0>(this);
            wzyy = swizzle<3, 2, 1, 1>(this);
            wzyz = swizzle<3, 2, 1, 2>(this);
            wzyw = swizzle<3, 2, 1, 3>(this);
            wzzx = swizzle<3, 2, 2, 0>(this);
            wzzy = swizzle<3, 2, 2, 1>(this);
            wzzz = swizzle<3, 2, 2, 2>(this);
            wzzw = swizzle<3, 2, 2, 3>(this);
            wzwx = swizzle<3, 2, 3, 0>(this);
            wzwy = swizzle<3, 2, 3, 1>(this);
            wzwz = swizzle<3, 2, 3, 2>(this);
            wzww = swizzle<3, 2, 3, 3>(this);
            wwxx = swizzle<3, 3, 0, 0>(this);
            wwxy = swizzle<3, 3, 0, 1>(this);
            wwxz = swizzle<3, 3, 0, 2>(this);
            wwxw = swizzle<3, 3, 0, 3>(this);
            wwyx = swizzle<3, 3, 1, 0>(this);
            wwyy = swizzle<3, 3, 1, 1>(this);
            wwyz = swizzle<3, 3, 1, 2>(this);
            wwyw = swizzle<3, 3, 1, 3>(this);
            wwzx = swizzle<3, 3, 2, 0>(this);
            wwzy = swizzle<3, 3, 2, 1>(this);
            wwzz = swizzle<3, 3, 2, 2>(this);
            wwzw = swizzle<3, 3, 2, 3>(this);
            wwwx = swizzle<3, 3, 3, 0>(this);
            wwwy = swizzle<3, 3, 3, 1>(this);
            wwwz = swizzle<3, 3, 3, 2>(this);
            wwww = swizzle<3, 3, 3, 3>(this);
        }
    };
}
