#pragma once
#include <string>
#include <initializer_list>
#include "Logging.h"
#include "Vector.h"
#include "Quaternion.h"
#include "EulerAngles.h"
#include "Utils.h"
#include "Plane.h"

namespace Starsurge {
    // Predefine classes due to circular inclusion.
    class Plane;
    class Quaternion;
    class EulerAngles;

    template<size_t M, size_t N = M>
    class Matrix {
    public:
        Matrix(float t_val = 0) {
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*N+c] = t_val;
                }
            }
        }

        template <typename = std::enable_if_t<(M==1)>>
        Matrix(Vector<N> v) {
            for (size_t c = 0; c < N; ++c) {
                this->data[c] = v[c];
            }
        }

        Matrix(const Matrix<M,N>& other) {
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*N+c] = other.data[r*N+c];
                }
            }
        }

        Matrix(float t_data[M*N]) {
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*N+c] = t_data[r*N+c];
                }
            }
        }

        Matrix(float t_data[M][N]) {
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*N+c] = t_data[r][c];
                }
            }
        }

        Matrix(std::initializer_list<float> list) {
            if (list.size() != M*N) {
                Error("Not correct amount of data.");
                for (size_t r = 0; r < M; ++r) {
                    for (size_t c = 0; c < N; ++c) {
                        this->data[r*N+c] = 0;
                    }
                }
                return;
            }
            size_t r = 0;
            size_t c = 0;
            for (auto it = std::begin(list); it != std::end(list); ++it) {
                this->data[r*N+c] = *it;
                c++;
                if (c == N) {
                    c = 0;
                    r++;
                }
            }
        }

        Matrix(std::initializer_list<Vector<M>> list) {
            if (list.size() != N) {
                Error("Not correct amount of data.");
                for (size_t r = 0; r < M; ++r) {
                    for (size_t c = 0; c < N; ++c) {
                        this->data[r*N+c] = 0;
                    }
                }
                return;
            }
            size_t i = 0;
            for (auto it = std::begin(list); it != std::end(list); ++it) {
                SetColumn(i++, *it);
            }
        }

        template <typename... Ts, typename = std::enable_if_t<(sizeof...(Ts)==M*N)>>
        Matrix(Ts... ts) {
            size_t i = 0;
            ((this->data[i++] = float(ts)),...);
        }

        size_t NumRows() const { return M; }
        size_t NumColumns() const { return N; }
        bool IsSquare() const { return M == N; }
        float * Ptr() { return &data[0]; }

        std::string ToString() const {
            std::string ret = "";
            for (size_t r = 0; r < M; ++r) {
                if (r > 0) ret += "\n";
                ret += "[ ";
                for (size_t c = 0; c < N; ++c) {
                    if (c > 0)
                        ret += ",";
                    ret += std::to_string(this->data[r*N+c]);
                }
                ret += " ]";
            }
            return ret;
        }

        Matrix<N,M> Transpose() {
            Matrix<N,M> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(c,r) = this->data[r*N+c];
                }
            }
            return ret;
        }

        template <size_t P>
        static Matrix<P,1> Transpose(Vector<P> v) {
            Matrix<P,1> ret;
            for (size_t r = 0; r < P; ++r) {
                ret(r,0) = v[r];
            }
            return ret;
        }

        template<size_t P, size_t R>
        Matrix<P,R> Resize() {
            return SubMatrix<P,R>(0,0);
        }

        template<size_t P, size_t R = P>
        Matrix<P,R> SubMatrix(size_t i, size_t j) {
            Matrix<P,R> ret;
            for (size_t r = 0; r < P; ++r) {
                for (size_t c = 0; c < R; ++c) {
                    if (r+i >= M || c+j >= N) {
                        ret(r,c) = 0;
                    }
                    else {
                        ret(r,c) = this->data[(r+i)*N+c+j];
                    }
                }
            }
            return ret;
        }

        float Get(size_t r, size_t c) {
            return this->data[r*N+c];
        }

        Vector<N> GetRow(size_t r) {
            Vector<N> ret;
            for (size_t c = 0; c < N; ++c) {
                ret[c] = this->data[r*N+c];
            }
            return ret;
        }

        Vector<M> GetColumn(size_t c) {
            Vector<M> ret;
            for (size_t r = 0; r < M; ++r) {
                ret[r] = this->data[r*N+c];
            }
            return ret;
        }

        void SetRow(size_t r, float v[N]) {
            for (size_t c = 0; c < N; ++c) {
                this->data[r*N+c] = v[c];
            }
        }

        void SetRow(size_t r, Vector<N> v) {
            for (size_t c = 0; c < N; ++c) {
                this->data[r*N+c] = v[c];
            }
        }

        void SetColumn(size_t c, float v[M]) {
            for (size_t r = 0; r < M; ++r) {
                this->data[r*N+c] = v[r];
            }
        }

        void SetColumn(size_t c, Vector<M> v) {
            for (size_t r = 0; r < M; ++r) {
                this->data[r*N+c] = v[r];
            }
        }

        Matrix<M-1,N> RemoveRow(size_t i) {
            Matrix<M-1,N> ret;
            size_t actualR = 0;
            for (size_t r = 0; r < M; ++r) {
                if (r == i) {
                    continue;
                }

                for (size_t c = 0; c < N; ++c) {
                    ret(actualR, c) = this->data[r*N+c];
                }
                actualR++;
            }
            return ret;
        }

        Matrix<M,N-1> RemoveColumn(size_t i) {
            Matrix<M,N-1> ret;
            for (size_t r = 0; r < M; ++r) {
                size_t actualC = 0;
                for (size_t c = 0; c < N; ++c) {
                    if (c == i) {
                        continue;
                    }

                    ret(r, actualC) = this->data[r*N+c];
                    actualC++;
                }
            }
            return ret;
        }

        Matrix<M-1,N-1> RemoveRowAndColumn(size_t i, size_t j) {
            Matrix<M-1,N-1> ret;
            size_t actualR = 0;
            for (size_t r = 0; r < M; ++r) {
                if (r == i) {
                    continue;
                }

                size_t actualC = 0;
                for (size_t c = 0; c < N; ++c) {
                    if (c == j) {
                        continue;
                    }

                    ret(actualR, actualC) = this->data[r*N+c];
                    actualC++;
                }
                actualR++;
            }
            return ret;
        }

        Matrix<M,N> RowReduce(size_t m = M, size_t n = N) {
            Matrix<M,N> ret(*this);

            size_t h = 0, k = 0;
            while (h < m && k < n) {
                // Find the k-th pivot.
                size_t i_max = h;
                float max = -1;
                for (size_t i = h; i < m; ++i) {
                    if (std::abs(ret(i, k)) > max) {
                        i_max = i;
                        max = std::abs(ret(i, k));
                    }
                }

                if (ret(i_max, k) == 0) { // No pivot in this column
                    k += 1;
                }
                else {
                    // Swap rows h and i_max
                    Vector<N> rowH = ret.GetRow(h);
                    Vector<N> rowIMax = ret.GetRow(i_max);
                    ret.SetRow(h, rowIMax);
                    ret.SetRow(i_max, rowH);

                    // Handle all rows below the pivot.
                    for (size_t i = h+1; i < M; ++i) {
                        float f = ret(i, k) / ret(h, k);
                        ret(i, k) = 0;

                        // Ri -> Ri - Rh*f
                        for (size_t j = k+1; j < N; ++j) {
                             ret(i, j) = ret(i,j) - ret(h,j)*f;
                        }
                    }

                    // Increase pivot row and column
                    h += 1;
                    k += 1;
                }
            }
            return ret;
        }

        Matrix<M,N> RREF() {
            Matrix<M,N> ret(*this);
            size_t lead = 0;
            for (size_t r = 0; r < M; ++r) {
                if (lead >= N) {
                    return ret;
                }

                // Find the pivot.
                size_t i = r;
                while (ret(i, lead) == 0) {
                    i += 1;
                    if (i == M) {
                        i = r;
                        lead += 1;
                        if (lead == N) {
                            return ret;
                        }
                    }
                }

                // Swap rows i and r.
                Vector<N> rowI = ret.GetRow(i);
                Vector<N> rowR = ret.GetRow(r);
                ret.SetRow(i, rowR);
                ret.SetRow(r, rowI);
                rowR = ret.GetRow(r);

                // Rr -> Rr / pivot
                ret.SetRow(r, rowR / ret(r, lead));
                rowR = ret.GetRow(r);

                // For each row i != r, Ri -> Ri - Rr*ret(i, lead)
                for (size_t i = 0; i < M; ++i) {
                    if (i == r)
                        continue;

                    rowI = ret.GetRow(i);
                    ret.SetRow(i, rowI - rowR * ret(i, lead));
                }

                // Increment lead.
                lead += 1;
            }

            return ret;
        }

        Matrix<M,N> RCEF() {
            Matrix<M,N> ret(*this);
            return ret.Transpose().RREF().Transpose();
        }

        std::vector<Vector<M>> Image(float eps = 0.00001) {
            Matrix<M,N> reduced = RREF();

            std::vector<Vector<M>> basis;
            for (size_t c = 0; c < N; c++) {
                Vector<M> col = reduced.GetColumn(c);
                if (std::abs(col[c]-1) < eps) {
                    basis.push_back(GetColumn(c));
                }
            }
            return basis;
        }

        unsigned int Rank(float eps = 0.00001) {
            std::vector<Vector<M>> basis = Image(eps);
            return basis.size();
        }

        std::vector<Vector<N>> Kernel(float eps = 0.00001) {
            Matrix<M+N,N> augmented = Matrix<M+N,N>::RowAugmented(*this, Matrix<N,N>::Identity());
            augmented = augmented.RCEF();
            Matrix<M,N> B = augmented.SubMatrix<M,N>(0,0);
            Matrix<N,N> C = augmented.SubMatrix<N,N>(M,0);

            std::vector<Vector<N>> basis;
            for (size_t c = 0; c < N; c++) {
                Vector<M> col = B.GetColumn(c);
                if (std::abs(col.Norm()) < eps) {
                    basis.push_back(C.GetColumn(c));
                }
            }
            return basis;
        }

        unsigned int Nullity(float eps = 0.00001) {
            std::vector<Vector<N>> basis = Kernel(eps);
            return basis.size();
        }

        static Matrix<M,N> Zero() {
            Matrix<M,N> ret(0);
            return ret;
        }

        static Matrix<M,N> One() {
            Matrix<M,N> ret(1);
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n), Matrix<N,N>>::type BackwardIdentity() {
            Matrix<N,N> ret(0);
            for (size_t r = 0; r < N; ++r) {
                ret(M-r-1,r) = 1;
            }
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n), Matrix<N,N>>::type LowerShift() {
            return Matrix<N,N>::SubDiag(Vector<N-1>::One());
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n), Matrix<N,N>>::type UpperShift() {
            return Matrix<N,N>::SuperDiag(Vector<N-1>::One());
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), bool>::type IsSymmetric() {
            return (*this == Transpose());
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), bool>::type IsOrthogonal() {
            return (Transpose()*(*this) == (*this)*Transpose() && Transpose()*(*this) == Matrix<M,N>::Identity());
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), bool>::type IsLowerTrianglar() {
            for (size_t r = 0; r < N; ++r) {
                for (size_t c = r+1; c < N; ++c) {
                    if (Get(r,c) != 0) {
                        return false;
                    }
                }
            }

            return true;
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), bool>::type IsUpperTrianglar() {
            for (size_t c = 0; c < N; ++c) {
                for (size_t r = c+1; r < N; ++r) {
                    if (Get(r,c) != 0) {
                        return false;
                    }
                }
            }

            return true;
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), bool>::type IsDiagonal() {
            return (IsUpperTrianglar() && IsLowerTrianglar());
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), bool>::type IsTriangular() {
            return (IsUpperTrianglar() || IsLowerTrianglar());
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), bool>::type IsPermutation() {
            std::vector<size_t> permutation;
            for (size_t c = 0; c < N; ++c) {
                Vector<N> col = GetColumn(c);
                for (size_t i = 0; i < N; ++i) {
                    if (col == Vector<N>::Basis(i)) {
                        if (ElemOf<size_t>(permutation, i)) {
                            return false;
                        }
                        permutation.push_back(i);
                        break;
                    }
                }

                if (permutation.size() != c+1) {
                    return false;
                }
            }

            return true;
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), Vector<N>>::type Solve(Vector<N> b) {
            Vector<N> x;
            Matrix<N,N> L, U, P;

            if (IsLowerTrianglar()) {
                for (size_t r = 0; r < N; ++r) {
                    float rhs = b[r];
                    for (size_t c = 0; c < r; ++c) {
                        rhs -= x[c]*Get(r,c);
                    }
                    x[r] = rhs / Get(r,r);
                }
                return x;
            }
            else if (IsUpperTrianglar()) {
                for (size_t r = N-1; r >= 0; r--) {
                    float rhs = b[r];
                    for (size_t c = N-1; c > r; c--) {
                        rhs -= x[c]*Get(r,c);
                    }
                    x[r] = rhs / Get(r,r);

                    if (r == 0) break;
                }
                return x;
            }
            else if (LUPDecomp(L, U, P)) {
                Vector<N> y = L.Solve(P*b);
                x = U.Solve(y);
                return x;
            }
            else {
                Error("Unable to solve equation Ax=b.");
                return x;
            }
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), void>::type QRDecomp(Matrix<N,N> & Q, Matrix<N,N> & R) {
            std::vector<Vector<N>> columns;
            for (size_t c = 0; c < N; ++c) {
                columns.push_back(GetColumn(c));
            }

            std::vector<Vector<N>> e = Vector<N>::GramSchmidt(columns);
            for (size_t c = 0; c < N; ++c) {
                Q.SetColumn(c, e[c]);
            }

            for (size_t r = 0; r < N; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    if (c < r) {
                        R(r,c) = 0;
                    }
                    else {
                        R(r,c) = Vector<N>::Dot(e[r], columns[c]);
                    }
                }
            }
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n), bool>::type LUPDecomp(Matrix<N,N> & L, Matrix<N,N> & U, Matrix<N,N> & P) {
            L = *this;

            std::vector<size_t> permutations;
            for (size_t i = 0; i < N; ++i) {
                permutations.push_back(i);
            }

            for (size_t i = 0; i < N; ++i) {
                float maxA = -1;
                size_t i_max = i;

                for (size_t k = i; k < N; ++k) {
                    if (std::abs(L(k,i)) > maxA) {
                        maxA = std::abs(L(k,i));
                        i_max = k;
                    }
                }

                if (maxA < 0.00001) {
                    return false;
                }

                if (i_max != i) {
                    float j = permutations[i];
                    permutations[i] = permutations[i_max];
                    permutations[i_max] = j;

                    Vector<N> rowI = L.GetRow(i);
                    L.SetRow(i, L.GetRow(i_max));
                    L.SetRow(i_max, rowI);
                }

                for (size_t j = i+1; j < N; ++j) {
                    L(j,i) /= L(i,i);

                    for (size_t k = i+1; k < N; ++k) {
                        L(j, k) -= L(j,i) * L(i,k);
                    }
                }
            }

            // Create the matrices.
            U = L;
            for (size_t r = 0; r < N; ++r) {
                L(r, r) = 1;
                for (size_t c = r+1; c < N; ++c) {
                    L(r, c) = 0;
                    U(c, r) = 0;
                }
            }

            for (size_t r = 0; r < N; ++r) {
                P.SetRow(r, Vector<N>::Basis(permutations[r]));
            }

            return true;
        }

        template<size_t P, size_t Q>
        static Matrix<P,Q> OuterProduct(Vector<P> u, Vector<Q> v) {
            return (Matrix<1,P>(u).Transpose())*(Matrix<1,Q>(v));
        }

        static Matrix<N,N> ProjectionMatrix(Vector<N> a) {
            a.Normalize();
            return OuterProduct(a, a);
        }

        static Matrix<N,N> RejectionMatrix(Vector<N> a) {
            a.Normalize();
            return Matrix<N,N>::Identity() - OuterProduct(a, a);
        }

        template<size_t P, size_t Q>
        static Matrix<M, P+Q> Augmented(Matrix<M,P> a, Matrix<M,Q> b) {
            Matrix<M,P+Q> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < P+Q; ++c) {
                    if (c < P)
                        ret(r,c) = a(r,c);
                    else
                        ret(r,c) = b(r,c-P);
                }
            }
            return ret;
        }

        template<size_t P, size_t Q>
        static Matrix<P+Q, N> RowAugmented(Matrix<P,N> a, Matrix<Q,N> b) {
            Matrix<P+Q,N> ret;
            for (size_t r = 0; r < P+Q; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    if (r < P)
                        ret(r,c) = a(r,c);
                    else
                        ret(r,c) = b(r-P,c);
                }
            }
            return ret;
        }

        // Transformations
        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n), Matrix<N,N>>::type Scale(Vector<N-1> s) {
            Vector<N> diag;
            for (size_t i = 0; i < N-1; ++i) {
                diag[i] = s[i];
            }
            diag[N-1] = 1;
            return Matrix<N,N>::Diag(diag);
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && (n == 3 || n == 4)), Matrix<N,N>>::type Scale(float s, Vector3 dir) {
            Matrix<4,4> ret = ((s-1)*Matrix<3,3>::ProjectionMatrix(dir)+Matrix<3,3>::Identity()).Resize<4,4>();
            ret(3,3) = 1;
            return ret.Resize<N,N>();
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n), Matrix<N,N>>::type Translate(Vector<N-1> t) {
            Matrix<N,N> ret = Matrix<N,N>::Identity();
            for (size_t i = 0; i < N-1; ++i) {
                ret(i, N-1) = t[i];
            }
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && N == 2), Matrix<2,2>>::type Rotate(float theta) {
            Matrix<2,2> ret = {
                std::cos(Radians(theta)), -std::sin(Radians(theta)),
                std::sin(Radians(theta)), std::cos(Radians(theta))
            };
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && (n == 3 || n == 4)), Matrix<N,N>>::type Rotate(Quaternion q) {
            // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
            q.Normalize();
            float q0 = q.scalar;
            float q1 = q.vector.x;
            float q2 = q.vector.y;
            float q3 = q.vector.z;
            Matrix<4,4> ret = {
                1-2*(q2*q2+q3*q3), 2*(q1*q2-q0*q3), 2*(q0*q2+q1*q3), 0,
                2*(q1*q2+q0*q3), 1-2*(q1*q1+q3*q3), 2*(q2*q3-q0*q1), 0,
                2*(q1*q3-q0*q2), 2*(q0*q1+q2*q3), 1-2*(q1*q1+q2*q2), 0,
                0, 0, 0, 1
            };
            return ret.Resize<N,N>();
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && (n == 3 || n == 4)), Matrix<N,N>>::type Rotate(EulerAngles euler) {
            switch (euler.GetOrder()) {
                case XYZ:
                    return Matrix<N,N>::RotateX(Radians(euler.x))*Matrix<N,N>::RotateY(Radians(euler.y))*Matrix<N,N>::RotateZ(Radians(euler.z));
                case YZX:
                    return Matrix<N,N>::RotateY(Radians(euler.y))*Matrix<N,N>::RotateZ(Radians(euler.z))*Matrix<N,N>::RotateX(Radians(euler.x));
                case ZXY:
                    return Matrix<N,N>::RotateZ(Radians(euler.z))*Matrix<N,N>::RotateX(Radians(euler.x))*Matrix<N,N>::RotateY(Radians(euler.y));
                case XZY:
                    return Matrix<N,N>::RotateX(Radians(euler.x))*Matrix<N,N>::RotateZ(Radians(euler.z))*Matrix<N,N>::RotateY(Radians(euler.y));
                case YXZ:
                    return Matrix<N,N>::RotateY(Radians(euler.y))*Matrix<N,N>::RotateX(Radians(euler.x))*Matrix<N,N>::RotateZ(Radians(euler.z));
                case ZYX:
                    return Matrix<N,N>::RotateZ(Radians(euler.z))*Matrix<N,N>::RotateY(Radians(euler.y))*Matrix<N,N>::RotateX(Radians(euler.x));
                default:
                    return Matrix<N,N>::Identity();
            };
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && (n == 3 || n == 4)), Matrix<N,N>>::type Rotate(float theta, Vector3 axis) {
            axis.Normalize();

            float c = std::cos(theta);
            float s = std::sin(theta);
            float rx = axis.x;
            float ry = axis.y;
            float rz = axis.z;
            Matrix<4,4> ret = {
                c+rx*rx*(1-c), rx*ry*(1-c)-rz*s, rx*rz*(1-c)+ry*s, 0,
                ry*rx*(1-c)+rz*s, c+ry*ry*(1-c), ry*rz*(1-c)-rx*s, 0,
                rz*rx*(1-c)-ry*s, rz*ry*(1-c)+rx*s, c+rz*rz*(1-c), 0,
                0, 0, 0, 1
            };
            return ret.Resize<N,N>();
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && (n == 3 || n == 4)), Matrix<N,N>>::type RotateX(float theta) {
            float c = std::cos(theta);
            float s = std::sin(theta);
            Matrix<4,4> ret = {
                1, 0, 0, 0,
                0, c, -s, 0,
                0, s, c, 0,
                0, 0, 0, 1
            };
            return ret.Resize<N,N>();
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && (n == 3 || n == 4)), Matrix<N,N>>::type RotateY(float theta) {
            float c = std::cos(theta);
            float s = std::sin(theta);
            Matrix<4,4> ret = {
                c, 0, s, 0,
                0, 1, 0, 0,
                -s, 0, c, 0,
                0, 0, 0, 1
            };
            return ret.Resize<N,N>();
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && (n == 3 || n == 4)), Matrix<N,N>>::type RotateZ(float theta) {
            float c = std::cos(theta);
            float s = std::sin(theta);
            Matrix<4,4> ret = {
                c, -s, 0, 0,
                s, c, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            };
            return ret.Resize<N,N>();
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<N,N>>::type Reflect(Plane plane) {
            Vector4 n = Vector4(plane.GetNormal().x, plane.GetNormal().y, plane.GetNormal().z, 0);
            Vector4 f = Vector4(plane.a, plane.b, plane.c, plane.d);
            return Matrix<4,4>::Identity() - 2 * Matrix<4,4>::OuterProduct(n, f);
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && (n == 3 || n == 4)), Matrix<N,N>>::type Skew(float theta, Vector<N> dir, Vector<N> perp) {
            Matrix<4,4> ret = (Matrix<3,3>::Identity() + std::tan(Radians(theta)) * Matrix<3,3>::OuterProduct(dir, perp)).Resize<4,4>();
            ret(3,3) = 1;
            return ret.Resize<N,N>();
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type Orthographic(float left,
                float right, float bottom, float top, float near, float far) {
            Matrix<4,4> ret;
            ret(0,0) = 2/(right-left);
            ret(1,1) = 2/(top-bottom);
            ret(2,2) = -2/(far-near);
            ret(3,3) = 1;
            ret(0,3) = -(right+left)/(right-left);
            ret(1,3) = -(top+bottom)/(top-bottom);
            ret(0,3) = -(far+near)/(far-near);
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type Perspective(float fov, float aspect, float near, float far) {
            float tanHalfFov = std::tan(fov / 2.0);

            Matrix<4,4> ret;
            ret(0,0) = 1.0/(aspect*tanHalfFov);
            ret(1,1) = 1.0/(tanHalfFov);
            ret(2,2) = -(far+near)/(far-near);
            ret(3,2) = -1.0;
            ret(2,3) = -(2.0*far*near)/(far-near);
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type LookAt(Vector3 right, Vector3 up, Vector3 dir, Vector3 eye) {
            Matrix<4,4> lhs = {
                right.x, right.y, right.z, 0,
                up.x, up.y, up.z, 0,
                dir.x, dir.y, dir.z, 0,
                0, 0, 0, 1
            };
            Matrix<4,4> rhs = {
                1, 0, 0, -eye.x,
                0, 1, 0, -eye.y,
                0, 0, 1, -eye.z,
                0, 0, 0, 1
            };
            return lhs * rhs;
        }

        // Square matrices
        template<size_t m = M, size_t n = N>
        static typename std::enable_if<m == n, Matrix<N,N>>::type Identity() {
            return Matrix<N,N>::Diag(Vector<N>(1));
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<m == n, Matrix<N,N>>::type Diag(const Vector<N>& vals) {
            Matrix<N,N> ret;
            for (size_t r = 0; r < N; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c)  = (r == c ? vals[r] : 0);
                }
            }
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<m == n, Matrix<N,N>>::type SuperDiag(const Vector<N-1>& vals) {
            Matrix<N,N> ret(0);
            for (size_t r = 0; r < N-1; ++r) {
                ret(r,r+1) = vals[r];
            }
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<m == n, Matrix<N,N>>::type SubDiag(const Vector<N-1>& vals) {
            Matrix<N,N> ret(0);
            for (size_t r = 0; r < N-1; ++r) {
                ret(r+1,r) = vals[r];
            }
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<m == n && n == 3, Matrix<3,3>>::type CrossProduct(Vector<3> v) {
            //https://en.wikipedia.org/wiki/Cross_product#Conversion_to_matrix_multiplication
            Matrix<3,3> ret = {
                0, -v.z, v.y,
                v.z, 0, -v.x,
                -v.y, v.x, 0
            };
            return ret;
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<m == n, float>::type Trace() {
            float sum = 0;
            for (size_t i = 0; i < N; ++i) {
                sum += this->data[i*N+i];
            }
            return sum;
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<m == n, bool>::type IsSingular() {
            return (Determinant() == 0);
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n && n == 1), float>::type Determinant() {
            return data[0];
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m == n && n == 2), float>::type Determinant() {
            return data[0]*data[3] - data[1]*data[2];
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<(m==n && n>2), float>::type Determinant() {
            if (IsTriangular()) {
                float det = 0;
                for (size_t i = 0; i < N; ++i) {
                    det *= Get(i, i);
                }
                return det;
            }

            float det = 0;
            for (size_t i = 0; i < N; ++i) {
                det += data[i] * Cofactor(0, i);
            }
            return det;
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<m == n, float>::type Minor(size_t i, size_t j) {
            return RemoveRowAndColumn(i, j).Determinant();
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<m == n, float>::type Cofactor(size_t i, size_t j) {
            return std::pow(-1.0f, i+j) * Minor(i, j);
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<m == n, Matrix<N,N>>::type Adjugate() {
            Matrix<N,N> ret;
            for (size_t r = 0; r < N; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Cofactor(r, c);
                }
            }
            return ret.Transpose();
        }

        template<size_t m = M, size_t n = N>
        typename std::enable_if<m == n, Matrix<N,N>>::type Inverse() {
            float det = Determinant();
            if (det == 0) {
                throw "Inverse of singular matrix.";
            }

            return (1/det)*Adjugate();
        }

        // operators
        float& operator() (size_t r, size_t c) { return this->data[r*N+c]; }
        const float& operator() (size_t r, size_t c) const { return this->data[r*N+c]; }

        Matrix<M,N>& operator=(const Matrix<M,N>& other) {
            if (this != &other) {
                for (size_t r = 0; r < M; ++r) {
                    for (size_t c = 0; c < N; ++c) {
                        this->data[r*N+c] = other(r,c);
                    }
                }
            }
            return *this;
        }
        friend bool operator==(const Matrix<M,N>& lhs, const Matrix<M,N>& rhs) {
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    if (lhs(r,c) != rhs(r,c))
                        return false;
                }
            }
            return true;
        }
        friend bool operator!=(const Matrix<M,N>& lhs, const Matrix<M,N>& rhs) { return !(lhs == rhs); }
        Matrix<M,N>& operator+=(const Matrix<M,N>& rhs) {
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*N+c] += rhs(r,c);
                }
            }
            return *this;
        }
        friend Matrix<M,N> operator+(Matrix<M,N> lhs, const Matrix<M,N>& rhs) { return lhs += rhs; }
        Matrix<M,N>& operator-=(const Matrix<M,N>& rhs) {
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*N+c] -= rhs(r,c);
                }
            }
            return *this;
        }
        friend Matrix<M,N> operator-(Matrix<M,N> lhs, const Matrix<M,N>& rhs) { return lhs -= rhs; }
        Matrix<M,N>& operator*=(const float& rhs) {
            for (size_t i = 0; i < M*N; ++i) {
                this->data[i] *= rhs;
            }
            return *this;
        }
        friend Matrix<M,N> operator*(Matrix<M,N> lhs, const float& rhs) { return lhs *= rhs; }
        friend Matrix<M,N> operator*(const float& lhs, Matrix<M,N> rhs) { return rhs *= lhs; }
        template<size_t P>
        friend Matrix<M,P> operator*(Matrix<M,N> lhs, Matrix<N,P> rhs) {
            Matrix<M,P> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < P; ++c) {
                    Vector<N> row = lhs.GetRow(r);
                    Vector<N> column = rhs.GetColumn(c);
                    ret(r,c) = Vector<N>::Dot(row, column);
                }
            }
            return ret;
        }
        friend Vector<M> operator*(Matrix<M,N> lhs, Vector<N> rhs) {
            Vector<M> ret;
            for (size_t r = 0; r < M; ++r) {
                Vector<N> row = lhs.GetRow(r);
                ret[r] = Vector<N>::Dot(row, rhs);
            }
            return ret;
        }
        Matrix<M,N>& operator*=(const Matrix<N,N>& rhs) {
            *this = *this * rhs;
            return *this;
        }
    protected:
        float data[M*N];
    };

    using Matrix2 = Matrix<2,2>;
    using Matrix3 = Matrix<3,3>;
    using Matrix4 = Matrix<4,4>;
}
