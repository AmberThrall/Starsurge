#pragma once
#include <string>
#include <tuple>
#include <type_traits>
#include <initializer_list>
#include "Logging.h"
#include "Utils.h"

namespace Starsurge {
    struct Empty {};

    const size_t Dynamic = 0;

    template<size_t N>
    class Vector {
    public:
        template <typename = std::enable_if_t<(N==Dynamic)>>
        Vector() { }
        template <typename = std::enable_if_t<(N!=Dynamic)>>
        Vector(float t_val = 0) {
            this->n = N;
            this->data = new float[this->n];
            for (size_t i = 0; i < this->n; ++i) {
                this->data[i] = t_val;
            }
        }
        template <typename = std::enable_if_t<(N==Dynamic)>>
        Vector(size_t size, float t_val = 0) {
            this->n = size;
            this->data = new float[this->n];
            for (size_t i = 0; i < this->n; ++i) {
                this->data[i] = t_val;
            }
        }
        Vector(const Vector<N>& other) {
            this->n = other.Size();
            this->data = new float[this->n];
            for (size_t i = 0; i < this->n; ++i) {
                this->data[i] = other.data[i];
            }
        }
        template <typename = std::enable_if_t<(N!=Dynamic)>>
        Vector(float t_data[N]) {
            this->n = N;
            this->data = new float[this->n];
            for (size_t i = 0; i < this->n; ++i) {
                this->data[i] = t_data[i];
            }
        }
        Vector(std::initializer_list<float> list) {
            this->n = N;
            if (N == Dynamic) {
                this->n = list.size();
            }
            this->data = new float[this->n];
            if (list.size() != this->n) {
                Error("Not correct amount of data.");
                for (size_t i = 0; i < this->n; ++i) {
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
        template <typename... Ts, typename = std::enable_if_t<(sizeof...(Ts)==N&&N!=Dynamic)>>
        Vector(Ts... ts) {
            size_t i = 0;
            this->n = N;
            if (N == Dynamic) { this->n = sizeof...(Ts); }
            this->data = new float[this->n];
            ((this->data[i++] = float(ts)),...);
        }
        template <size_t M,typename... Ts, typename = std::enable_if_t<(M+sizeof...(Ts)==N||N==Dynamic||M==Dynamic)>>
        Vector(const Vector<M> v, Ts... ts) {
            size_t i = 0;
            this->n = N;
            if (N == Dynamic) {
                this->n = v.Size()+sizeof...(Ts);
            }
            this->data = new float[this->n];

            if (this->n != v.Size()+sizeof...(Ts)) {
                Error("Not correct amount of data.");
                for (size_t i = 0; i < this->n; ++i) {
                    this->data[i] = 0;
                }
                return;
            }
            for (; i < v.Size(); ++i) {
                this->data[i] = v.data[i];
            }
            ((this->data[i++] = float(ts)),...);
        }

        size_t Size() const { return this->n; }

        float Head() const {
            return this->data[0];
        }
        float Tail() const {
            return this->data[this->n-1];
        }

        template <size_t M, typename = std::enable_if_t<(M!=Dynamic)>>
        Vector<M> Resize() const {
            return SubVector<M>(0);
        }
        Vector<Dynamic> Resize(size_t size) const {
            return SubVector(0, size);
        }

        template <size_t M, typename = std::enable_if_t<(M!=Dynamic)>>
        Vector<M> SubVector(size_t i) const {
            Vector<M> ret;
            for (size_t j = i; j < M && j < this->n; ++j) {
                ret[j] = this->data[j];
            }
            return ret;
        }
        Vector<Dynamic> SubVector(size_t i, size_t size) const {
            Vector<Dynamic> ret(size, 0);
            for (size_t j = i; j < size && j < this->n; ++j) {
                ret[j] = this->data[j];
            }
            return ret;
        }

        std::string ToString(unsigned int ndigits = 3) const {
            std::string ret = "[";
            for (size_t i = 0; i < this->n; ++i) {
                if (i > 0)
                    ret += ",";
                ret += Starsurge::ToString(this->data[i], ndigits, false);
            }
            ret += "]";
            return ret;
        }

        float Norm() const { return Magnitude(); }
        float Length() const { return Magnitude(); }
        float Magnitude() const {
            return std::sqrt(SquaredMagnitude());
        }
        float SquaredNorm() const { return SquaredMagnitude(); }
        float SquaredLength() const { return SquaredMagnitude(); }
        float SquaredMagnitude() const {
            return Vector::Dot(*this, *this);
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

        bool All() const {
            for (size_t i = 0; i < this->n; ++i) {
                if (this->data[i] <= 0) {
                    return false;
                }
            }
            return true;
        }

        bool Any() const {
            for (size_t i = 0; i < this->n; ++i) {
                if (this->data[i] > 0) {
                    return true;
                }
            }
            return false;
        }

        unsigned int Count() const {
            unsigned int count = 0;
            for (size_t i = 0; i < this->n; ++i) {
                if (this->data[i] > 0) {
                    count += 1;
                }
            }
            return count;
        }

        Vector<N> Not() const {
            Vector<N> ret = CreateVector<N>(this->n, 0);
            for (size_t i = 0; i < this->n; ++i) {
                if (this->data[i] > 0) {
                    ret[i] = 0;
                }
                else {
                    ret[i] = 1;
                }
            }
            return ret;
        }

        template<typename = std::enable_if_t<(N!=Dynamic)>>
        static Vector<N> Zero() {
            return CreateVector<N>(N, 0);
        }
        static Vector<Dynamic> Zero(size_t size) {
            return CreateVector<Dynamic>(size, 0);
        }
        template<typename = std::enable_if_t<(N!=Dynamic)>>
        static Vector<N> One() {
            return CreateVector<N>(N, 1);
        }
        static Vector<Dynamic> One(size_t size) {
            return CreateVector<Dynamic>(size, 1);
        }

        template <typename = std::enable_if_t<(N==3)>>
        static Vector<3> Right() {
            return Vector<3>(1,0,0);
        }
        template <typename = std::enable_if_t<(N==3)>>
        static Vector<3> Up() {
            return Vector<3>(0,1,0);
        }
        template <typename = std::enable_if_t<(N==3)>>
        static Vector<3> Forward() {
            return Vector<3>(0,0,1);
        }

        template<typename = std::enable_if_t<(N!=Dynamic)>>
        static Vector<N> Linspace(float start, float stop, bool endpoint = true) {
            float step = (stop - start) / N;
            if (endpoint) {
                step = (stop - start) / (N-1);
            }
            float v = start;
            Vector<N> ret;
            ret[0] = v;
            for (size_t i = 1; i < N; ++i) {
                v += step;
                ret[i] = v;
            }
            return ret;
        }
        static Vector<Dynamic> Linspace(float start, float stop, size_t size, bool endpoint = true) {
            float step = (stop - start) / size;
            if (endpoint) {
                step = (stop - start) / (size-1);
            }
            float v = start;
            Vector<Dynamic> ret = CreateVector(size, 0);
            ret[0] = v;
            for (size_t i = 1; i < size; ++i) {
                v += step;
                ret[i] = v;
            }
            return ret;
        }

        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> EntrywiseProduct(Vector<P> a, Vector<Q> b) {
            if (a.Size() != b.Size()) {
                throw "Cannot take the entrywise product of vectors with varying length.";
            }
            Vector<P> ret = CreateVector<P>(a.Size(), 0);
            for (size_t i = 0; i < a.Size(); ++i) {
                ret[i] = a[i]*b[i];
            }
            return ret;
        }

        static float Sum(Vector<N> a) {
            float sum = 0;
            for (size_t i = 0; i < a.Size(); ++i) {
                sum += a[i];
            }
            return sum;
        }

        static float Product(Vector<N> a) {
            float prod = 1;
            for (size_t i = 0; i < a.Size(); ++i) {
                prod *= a[i];
            }
            return prod;
        }

        static float Mean(Vector<N> a) {
            return Vector<N>::Sum(a) / a.Size();
        }

        static float Min(Vector<N> a) {
            float min = a[0];
            for (size_t i = 0; i < a.Size(); ++i) {
                min = Starsurge::Min(min, a[i]);
            }
            return min;
        }

        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> Min(Vector<P> a, Vector<Q> b) {
            if (a.Size() != b.Size()) {
                throw "Cannot take the minimum of vectors with varying length.";
            }
            Vector<P> ret = CreateVector<P>(a.Size(), 0);
            for (size_t i = 0; i < a.Size(); ++i) {
                ret[i] = Starsurge::Min(a[i], b[i]);
            }
            return ret;
        }

        template <size_t P, size_t Q, typename...Ts>
        static Vector<P> Min(Vector<P> a, Vector<Q> b, Ts&&... ts) {
            Vector<P> ret = Vector<P>::Min(a,b);
            return Vector<P>::Min(ret, std::forward<Ts>(ts)...);
        }

        static float Max(Vector<N> a) {
            float max = a[0];
            for (size_t i = 0; i < this->n; ++i) {
                max = Starsurge::Max(max, a[i]);
            }
            return max;
        }

        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> Max(Vector<P> a, Vector<Q> b) {
            if (a.Size() != b.Size()) {
                throw "Cannot take the maximum of vectors with varying length.";
            }
            Vector<P> ret = CreateVector<P>(a.Size(), 0);
            for (size_t i = 0; i < a.Size(); ++i) {
                ret[i] = Starsurge::Max(a[i], b[i]);
            }
            return ret;
        }

        template <size_t P, size_t Q, typename...Ts>
        static Vector<P> Max(Vector<P> a, Vector<Q> b, Ts&&... ts) {
            Vector<P> ret = Vector<P>::Max(a,b);
            return Vector<P>::Max(ret, std::forward<Ts>(ts)...);
        }

        template <size_t P, size_t Q, size_t R, typename = std::enable_if_t<((P==Q && P==R)||P==Dynamic||Q==Dynamic||N==Dynamic)>>
        static Vector<P> Clamp(Vector<P> a, Vector<Q> min, Vector<R> max) {
            if (a.Size() != min.Size() || a.Size() != max.Size()) {
                throw "Cannot take the clamp of a vector with differing dimensions min and max.";
            }
            return Vector<P>::Max(Vector<P>::Min(a, max), min);
        }

        static Vector<N> Abs(Vector<N> a) {
            Vector<N> ret = CreateVector<N>(a.Size(), 0);
            for (size_t i = 0; i < a.Size(); ++i) {
                ret[i] = std::abs(a[i]);
            }
            return ret;
        }

        static Vector<N> Floor(Vector<N> a) {
            Vector<N> ret = CreateVector<N>(a.Size(), 0);
            for (size_t i = 0; i < a.Size(); ++i) {
                ret[i] = std::floor(a[i]);
            }
            return ret;
        }

        static Vector<N> Ceil(Vector<N> a) {
            Vector<N> ret = CreateVector<N>(a.Size(), 0);
            for (size_t i = 0; i < a.Size(); ++i) {
                ret[i] = std::ceil(a[i]);
            }
            return ret;
        }

        static Vector<N> Round(Vector<N> a) {
            Vector<N> ret = CreateVector<N>(a.Size(), 0);
            for (size_t i = 0; i < a.Size(); ++i) {
                ret[i] = std::round(a[i]);
            }
            return ret;
        }

        static Vector<N> Fract(Vector<N> a) {
            return a - Vector<N>::Floor(a);
        }

        static Vector<N> Mod(Vector<N> a, float b) {
            Vector<N> ret = CreateVector<N>(a.Size(), 0);
            for (size_t i = 0; i < this->n; ++i) {
                ret[i] = std::fmod(a[i], b);
            }
            return ret;
        }

        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> Mix(Vector<P> a, Vector<Q> b, float amt) {
            return (1-amt)*a+amt*b;
        }

        /* Returns true if a and b point in the same direction, i.e., a = tb, t >= 0. */
        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static bool SameDirection(Vector<P> a, Vector<Q> b, float eps = 0.00001) {
            Vector<P> c = Vector<Q>::Normalize(b) - Vector<P>::Normalize(a);
            if (c.Norm() < eps) {
                return true;
            }
            return false;
        }

        /* Returns true if a is parallel to b, i.e., a = tb. */
        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static bool Parallel(Vector<P> a, Vector<Q> b, float eps = 0.00001) {
            return SameDirection(a,b) || SameDirection(a, -1.0f*b);
        }

        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> Lerp(Vector<P> start, Vector<Q> end, float t) {
            t = Starsurge::Clamp(t, 0, 1);
            return start + t*(end - start);
        }
        static Vector<N> Spline(Vector<N> p0, Vector<N> m0, Vector<N> p1, Vector<N> m1, float t) {
            t = Starsurge::Clamp(t, 0, 1);
            return (1+2*t)*(1-t)*(1-t)*p0 + t*(1-t)*(1-t)*m0 + t*t*(3-2*t)*p1+t*t*(t-1)*m1;
        }

        static Vector<N> Normalize(const Vector<N>& v) {
            Vector<N> ret(v);
            ret.Normalize();
            return ret;
        }

        // Vector projection of a onto b. a_{∥b} or proj_b a
        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> Projection(const Vector<P> a, const Vector<Q> b) {
            return (Vector<P>::Dot(a,b)/Vector<P>::Dot(b,b))*b;
        }
        // Vector rejection of a from b. a_{⊥b} or oproj_b a
        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> Rejection(const Vector<P> a, const Vector<Q> b) {
            return a - Vector<P>::Projection(a, b);
        }
        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> Reflect(const Vector<P> incident, Vector<Q> normal) {
            // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/reflect.xhtml
            return incident - 2.0f * Vector<P>::Dot(normal, incident) * normal;
        }
        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static Vector<P> Refract(const Vector<P> incident, Vector<Q> normal, float eta) {
            // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/refract.xhtml
            float k = 1.0 - eta*eta * (1.0 - std::pow(Vector<P>::Dot(normal, incident), 2))
            if (k < 0.0) {
                return Vector<P>::Zero();
            }
            return eta * incident - (eta * Vector<P>::Dot(normal, incident) + std::sqrt(k))*normal;
        }
        template <size_t P, size_t Q, size_t R, typename = std::enable_if_t<((P==Q&&P==R)||P==Dynamic||Q==Dynamic||R==Dynamic)>>
        static Vector<P> FaceForward(const Vector<P> v, Vector<Q> incident, Vector<R> ref) {
            // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/faceforward.xhtml
            if (Vector<P>::Dot(ref, incident) < 0) {
                return v;
            }
            return -v;
        }

        /* Skew the vector in the direction 'dir' by 'theta' degrees. It is assumed that 'perp' is perpindicular to 'dir'. */
        template <size_t P, size_t Q, size_t R, typename = std::enable_if_t<((P==Q&&P==R)||P==Dynamic||Q==Dynamic||R==Dynamic)>>
        static Vector<P> Skew(const Vector<P> v, Vector<Q> dir, Vector<R> perp, float theta) {
            dir.Normalize();
            perp.Normalize();
            return v + dir * Vector<P>::Dot(perp, v) * std::tan(Radians(theta));
        }

        static std::vector<Vector<N>> GramSchmidt(std::vector<Vector<N>> vectors) {
            std::vector<Vector<N>> u;
            for (size_t i = 0; i < vectors.size(); ++i) {
                Vector<N> ui = vectors[i];
                for (size_t j = 0; j < i; j++) {
                    ui -= Vector<N>::Projection(u[j], vectors[i]);
                }
                u.push_back(ui);
            }

            // Normalize all the u's.
            for (size_t i = 0; i < u.size(); ++i) {
                u[i].Normalize();
            }

            return u;
        }

        static float TripleProduct(const Vector<3>& a, const Vector<3>& b, const Vector<3>& c) {
            // https://en.wikipedia.org/wiki/Triple_product
            return Vector3::Dot(a, Vector3::CrossProduct(b, c));
        }
        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static float Dot(const Vector<P>& lhs, const Vector<Q>& rhs) {
            if (lhs.Size() != rhs.Size()) {
                throw "Cannot take the dot product of two vector with differing dimensions.";
            }
            float ret = 0;
            for (size_t i = 0; i < lhs.Size(); ++i) {
                ret += lhs[i]*rhs[i];
            }
            return ret;
        }

        template <typename = std::enable_if_t<(N!=Dynamic)>>
        static Vector<N> Basis(size_t i) {
            Vector<N> ret;
            ret[i] = 1;
            return ret;
        }
        static Vector<Dynamic> Basis(size_t dim, size_t i) {
            Vector<Dynamic> ret = CreateVector<Dynamic>(dim,0);
            ret[i] = 1;
            return ret;
        }

        static Vector<3> CrossProduct(Vector<3> a, Vector<3> b) {
            float x = a[1]*b[2] - a[2]*b[1];
            float y = -(a[0]*b[2] - a[2]*b[0]);
            float z = a[0]*b[1] - a[1]*b[0];
            return Vector<3>(x, y, z);
        }
        template <size_t P, size_t Q, typename = std::enable_if_t<(P==Q||P==Dynamic||Q==Dynamic)>>
        static float AngleBetween(Vector<P> a, Vector<Q> b) {
            // a DOT b = ||a||*||b||*cos(theta)
            float dot = Vector<P>::Dot(a, b);
            float normA = a.Norm();
            float normB = b.Norm();
            return Degrees(std::acos(dot/(normA*normB)));
        }

        void Set(size_t i, float v) {
            this->data[i] = v;
        }
        float Get(size_t i) {
            return this->data[i];
        }

        // Operators:
        Vector<N>& operator=(const Vector<N>& other) { //Copy operator
            for (size_t i = 0; i < this->n; ++i) {
                this->data[i] = other.data[i];
            }
            return *this;
        }
        Vector<N>& operator=(Vector<N>&& other) { //Move operator
            this->data = std::move(other.data);
            return *this;
        }
        float operator [](int i) const { return this->data[i]; }
        float & operator [](int i) { return this->data[i]; }
        template <size_t P, typename = std::enable_if_t<(P==N||P==Dynamic||N==Dynamic)>>
        Vector<N>& operator+=(const Vector<P>& rhs) {
            if (Size() != rhs.Size()) {
                throw "Cannot add vectors of differing size.";
            }
            for (size_t i=0; i < this->n; ++i) {
                this->data[i] += rhs[i];
            }
            return *this;
        }
        friend Vector<N> operator+(Vector<N> lhs, const Vector<N>& rhs) { return lhs += rhs; }
        template <size_t P, typename = std::enable_if_t<(P==N||P==Dynamic||N==Dynamic)>>
        Vector<N>& operator-=(const Vector<P>& rhs) {
            if (Size() != rhs.Size()) {
                throw "Cannot subtract vectors of differing size.";
            }
            for (size_t i=0; i < this->n; ++i) {
                this->data[i] -= rhs[i];
            }
            return *this;
        }
        friend Vector<N> operator-(const Vector<N>& rhs) { return -1.0f * rhs; }
        friend Vector<N> operator-(Vector<N> lhs, const Vector<N>& rhs) { return lhs -= rhs; }
        Vector<N>& operator*=(const float& rhs) {
            for (size_t i=0; i < this->n; ++i) {
                this->data[i] *= rhs;
            }
            return *this;
        }
        friend Vector<N> operator*(Vector<N> lhs, const float& rhs) { return lhs *= rhs; }
        friend Vector<N> operator*(const float& lhs, Vector<N> rhs) { return rhs *= lhs; }
        Vector<N>& operator/=(const float& rhs) {
            for (size_t i=0; i < this->n; ++i) {
                this->data[i] /= rhs;
            }
            return *this;
        }
        friend Vector<N> operator/(Vector<N> lhs, const float& rhs) { return lhs /= rhs; }
        friend bool operator==(const Vector<N>& lhs, const Vector<N>& rhs) {
            if (lhs.Size() != rhs.Size()) {
                return false;
            }
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] != rhs[i]) {
                    return false;
                }
            }
            return true;
        }
        friend bool operator!=(const Vector<N>& lhs, const Vector<N>& rhs) { return !(lhs == rhs); }
        friend Vector<N> operator==(const Vector<N>& lhs, float rhs) {
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < this->n; ++i) {
                if (lhs[i] == rhs) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator!=(const Vector<N>& lhs, float rhs) {
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] != rhs) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator>(const Vector<N>& lhs, const Vector<N>& rhs) {
            if (lhs.Size() != rhs.Size()) {
                throw "Cannot compare vectors of differing size.";
            }
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] > rhs[i]) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator>(const Vector<N>& lhs, float rhs) {
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] > rhs) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator>=(const Vector<N>& lhs, const Vector<N>& rhs) {
            if (lhs.Size() != rhs.Size()) {
                throw "Cannot compare vectors of differing size.";
            }
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] >= rhs[i]) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator>=(const Vector<N>& lhs, float rhs) {
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] >= rhs) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator<(const Vector<N>& lhs, const Vector<N>& rhs) {
            if (lhs.Size() != rhs.Size()) {
                throw "Cannot compare vectors of differing size.";
            }
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] < rhs[i]) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator<(const Vector<N>& lhs, float rhs) {
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] < rhs) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator<=(const Vector<N>& lhs, const Vector<N>& rhs) {
            if (lhs.Size() != rhs.Size()) {
                throw "Cannot compare vectors of differing size.";
            }
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] <= rhs[i]) {
                    ret[i] = 1;
                }
            }
            return ret;
        }
        friend Vector<N> operator<=(const Vector<N>& lhs, float rhs) {
            Vector<N> ret = CreateVector<N>(lhs.Size(), 0);
            for (size_t i=0; i < lhs.Size(); ++i) {
                if (lhs[i] <= rhs) {
                    ret[i] = 1;
                }
            }
            return ret;
        }

        // Swizzling
        template <size_t idx>
        class swizzle {
        public:
            operator float() const {
                return m_data[idx];
            }
            swizzle<idx>& operator=(const float& other) {
                m_data[idx] = other;
                return (*this);
            }
            template<size_t uidx>
            swizzle<idx>& operator=(const swizzle<uidx>& other) {
                m_data = other.m_data[uidx];
                return *this;
            };
            swizzle<idx>& operator=(swizzle<idx>&& other) = delete;
            swizzle<idx>& operator++(int) {
                m_data[idx]++;
                return (*this);
            }
            swizzle<idx>& operator++() {
                ++m_data[idx];
                return (*this);
            }
            swizzle<idx>& operator--(int) {
                m_data[idx]--;
                return (*this);
            }
            swizzle<idx>& operator--() {
                --m_data[idx];
                return (*this);
            }
            swizzle<idx>& operator+=(const float& other) {
                m_data[idx] += other;
                return (*this);
            }
            swizzle<idx>& operator-=(const float& other) {
                m_data[idx] -= other;
                return (*this);
            }
            swizzle<idx>& operator*=(const float& other) {
                m_data[idx] *= other;
                return (*this);
            }
            swizzle<idx>& operator/=(const float& other) {
                m_data[idx] /= other;
                return (*this);
            }
        private:
            float * m_data;
        };

        union {
            float * data;
            std::conditional_t<(N==2 || N==3 || N==4), swizzle<0>, Empty> x, r, s;
            std::conditional_t<(N==2 || N==3 || N==4), swizzle<1>, Empty> y, g, t;
            std::conditional_t<(N==3 || N==4), swizzle<2>, Empty> z, b, p;
            std::conditional_t<(N==4), swizzle<3>, Empty> w, a;
        };
    private:
        size_t n;
        template <size_t P>
        static typename std::enable_if<(P==Dynamic), Vector<P>>::type CreateVector(size_t size, float value = 0) {
            Vector<Dynamic> ret(size, value);
            return ret;
        }
        template <size_t P>
        static typename std::enable_if<(P!=Dynamic), Vector<P>>::type CreateVector(size_t size, float value = 0) {
            Vector<P> ret(value);
            return ret;
        }
    };

    using Vector2 = Vector<2>;
    using Vector3 = Vector<3>;
    using Vector4 = Vector<4>;
}
