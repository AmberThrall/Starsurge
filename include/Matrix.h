#pragma once
#include <string>
#include <initializer_list>
#include "Logging.h"
#include "Vector.h"

namespace Starsurge {
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

        Matrix<M,N> RowReduce() {
            Matrix<M,N> ret(*this);

            size_t lead = 0;
            for (size_t r = 0; r < M; ++r) {
                if (lead >= N)
                    break;

                // Find the first non-zero pivot.
                size_t i = r;
                while (ret(i, lead) == 0) {
                    i++;
                    if (i >= M) {
                        i = r;
                        lead++;
                        if (lead >= N)
                            return ret;
                    }
                }

                // Swap rows i and r.
                Vector<N> rowI = ret.GetRow(i);
                Vector<N> rowR = ret.GetRow(r);
                ret.SetRow(i, rowR);
                ret.SetRow(r, rowI);

                // Normalize the pivot.
                if (ret(r, lead) != 0 && ret(r, lead) != 1) {
                    rowR = ret.GetRow(r);
                    ret.SetRow(r, rowR*(1/ret(r,lead)));
                }

                // Cancel out rows.
                for (size_t i = 0; i < M; ++i) {
                    if (i != r) {
                        rowR = ret.GetRow(r);
                        rowI = ret.GetRow(i);
                        ret.SetRow(i, -ret(i,lead)*rowR+rowI);
                    }
                }

                lead++;
            }

            return ret;
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

        // Transformations
        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type Scale(Vector3 s) {
            Matrix<4,4> ret = {
                s.x, 0, 0, 0,
                0, s.y, 0, 0,
                0, 0, s.z, 0,
                0, 0, 0, 1
            };
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type Translate(Vector3 t) {
            Matrix<4,4> ret = {
                1, 0, 0, t.x,
                0, 1, 0, t.y,
                0, 0, 1, t.z,
                0, 0, 0, 1
            };
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type Rotate(Vector3 eulerAngles) {
            float cx = std::cos(eulerAngles.x);
            float cy = std::cos(eulerAngles.y);
            float cz = std::cos(eulerAngles.z);
            float sx = std::sin(eulerAngles.x);
            float sy = std::sin(eulerAngles.y);
            float sz = std::sin(eulerAngles.z);
            Matrix<4,4> ret = {
                cx*cy, cx*sy*sz-sx*cz, cx*sy*cz+sx*sz, 0,
                sx*cy, sx*sy*sz+cx*cz, sx*sy*cz-cx*sz, 0,
                -sy, cy*sz, cy*cz, 0,
                0, 0, 0, 1
            };
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type Rotate(float theta, Vector3 axis) {
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
            return ret;
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

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type RotateX(float theta) {
            return Matrix<4,4>::Rotate(theta, Vector3(1,0,0));
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type RotateY(float theta) {
            return Matrix<4,4>::Rotate(theta, Vector3(0,1,0));
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n && n == 4), Matrix<4,4>>::type RotateZ(float theta) {
            return Matrix<4,4>::Rotate(theta, Vector3(0,0,1));
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
    protected:
        float data[M*N];
    };

    using Matrix2 = Matrix<2,2>;
    using Matrix3 = Matrix<3,3>;
    using Matrix4 = Matrix<4,4>;
}
