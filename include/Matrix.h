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
        template <typename = std::enable_if_t<(M==Dynamic || N==Dynamic)>>
        Matrix() { }

        template <typename = std::enable_if_t<(M!=Dynamic && N!=Dynamic)>>
        Matrix(float t_val = 0) {
            this->m = M;
            this->n = N;
            this->data = new float[m*n];
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*this->n+c] = t_val;
                }
            }
        }

        template <typename = std::enable_if_t<(M==Dynamic && N==Dynamic)>>
        Matrix(size_t _m, size_t _n, float t_val = 0) {
            this->m = _m;
            this->n = _n;
            this->data = new float[this->m*this->n];
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    this->data[r*this->n+c] = t_val;
                }
            }
        }

        template <typename = std::enable_if_t<((M==Dynamic&&N!=Dynamic)||(M!=Dynamic&&N==Dynamic))>>
        Matrix(size_t _size, float t_val = 0) {
            if (M == Dynamic) { this->m = _size; this->n = N; }
            if (N == Dynamic) { this->n = _size; this->m = M; }
            this->data = new float[this->m*this->n];
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    this->data[r*this->n+c] = t_val;
                }
            }
        }

        template <typename = std::enable_if_t<(M==1 || M==Dynamic)>>
        Matrix(Vector<N> v) {
            this->m = 1;
            this->n = v.Size();
            this->data = new float[this->m*this->n];
            for (size_t c = 0; c < this->n; ++c) {
                this->data[c] = v[c];
            }
        }

        Matrix(const Matrix<M,N>& other) {
            this->m = other.NumRows();
            this->n = other.NumColumns();
            this->data = new float[this->m*this->n];
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    this->data[r*this->n+c] = other.data[r*this->n+c];
                }
            }
        }

        template <typename = std::enable_if_t<(M!=Dynamic && N!=Dynamic)>>
        Matrix(float t_data[M*N]) {
            this->m = M;
            this->n = N;
            this->data = new float[this->m*this->n];
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*this->n+c] = t_data[r*this->n+c];
                }
            }
        }

        template <typename = std::enable_if_t<(M!=Dynamic && N!=Dynamic)>>
        Matrix(float t_data[M][N]) {
            this->m = M;
            this->n = N;
            this->data = new float[this->m*this->n];
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    this->data[r*this->n+c] = t_data[r][c];
                }
            }
        }

        template <typename = std::enable_if_t<(M!=Dynamic && N!=Dynamic)>>
        Matrix(std::initializer_list<float> list) {
            this->m = M;
            this->n = N;
            this->data = new float[this->m*this->n];
            if (list.size() != this->m*this->n) {
                Error("Not correct amount of data.");
                for (size_t r = 0; r < this->m; ++r) {
                    for (size_t c = 0; c < this->n; ++c) {
                        this->data[r*this->n+c] = 0;
                    }
                }
                return;
            }
            size_t r = 0;
            size_t c = 0;
            for (auto it = std::begin(list); it != std::end(list); ++it) {
                this->data[r*this->n+c] = *it;
                c++;
                if (c == this->n) {
                    c = 0;
                    r++;
                }
            }
        }

        Matrix(std::initializer_list<Vector<M>> list) {
            this->m = M;
            this->n = N;
            if (M == Dynamic) {
                this->m = (*std::begin(list)).Size();
            }
            if (N == Dynamic) {
                this->n = list.size();
            }
            this->data = new float[this->m*this->n];
            if (list.size() != this->n) {
                Error("Not correct amount of data.");
                for (size_t r = 0; r < this->m; ++r) {
                    for (size_t c = 0; c < this->n; ++c) {
                        this->data[r*this->n+c] = 0;
                    }
                }
                return;
            }
            size_t i = 0;
            for (auto it = std::begin(list); it != std::end(list); ++it) {
                SetColumn(i++, *it);
            }
        }

        size_t NumRows() const { return this->m; }
        size_t NumColumns() const { return this->n; }
        bool IsSquare() const { return this->m == this->n; }
        float * Ptr() { return &data[0]; }

        bool All() const {
            for (size_t i = 0; i < this->m*this->n; ++i) {
                if (this->data[i] <= 0) {
                    return false;
                }
            }
            return true;
        }

        bool Any() const {
            for (size_t i = 0; i < this->m*this->n; ++i) {
                if (this->data[i] > 0) {
                    return true;
                }
            }
            return false;
        }

        Matrix<M,N> Not() const {
            Matrix<M,N> ret = CreateMatrix<M,N>(this->m, this->n);
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    if (this->data[r*this->n+c] > 0) {
                        ret(r,c) = 0;
                    }
                    else {
                        ret(r,c) = 1;
                    }
                }
            }
            return ret;
        }

        unsigned int Count() const {
            unsigned int count = 0;
            for (size_t i = 0; i < this->m*this->n; ++i) {
                if (this->data[i] > 0) {
                    count += 1;
                }
            }
            return count;
        }

        std::string ToString(unsigned int ndigits = 3) const {
            std::string ret = std::to_string(NumRows())+"x"+std::to_string(NumColumns());

            // Get the largest sized (string-wise) cell.
            const size_t spacing = 4;
            size_t maxLength = 0;
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    size_t entryLength = Starsurge::ToString(this->data[r*this->n+c], ndigits).length();
                    if (entryLength > maxLength) {
                        maxLength = entryLength;
                    }
                }
            }
            for (size_t r = 0; r < this->m; ++r) {
                ret += "\n";
                for (size_t c = 0; c < this->n; ++c) {
                    std::string entry = Starsurge::ToString(this->data[r*this->n+c], ndigits);
                    for (size_t spaces = entry.length(); spaces < maxLength+spacing; ++spaces) { ret += " "; }
                    ret += entry;
                }
            }
            return ret;
        }

        Matrix<N,M> Transpose() {
            Matrix<N,M> ret = CreateMatrix<N,M>(this->n, this->m);
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    ret(c,r) = this->data[r*this->n+c];
                }
            }
            return ret;
        }

        template <size_t P>
        static Matrix<P,1> Transpose(Vector<P> v) {
            Matrix<P,1> ret = CreateMatrix<P,1>(v.Size(), 1);
            for (size_t r = 0; r < v.Size(); ++r) {
                ret(r,0) = v[r];
            }
            return ret;
        }

        template<size_t P, size_t R = P, typename = std::enable_if_t<(P!=Dynamic&&R!=Dynamic)>>
        Matrix<P,R> Resize() {
            return SubMatrix<P,R>(0,0);
        }
        Matrix<Dynamic,Dynamic> Resize(size_t size_m, size_t size_n) {
            return SubMatrix(0,0,size_m,size_n);
        }

        template<size_t P, size_t R = P, typename = std::enable_if_t<(P!=Dynamic&&R!=Dynamic)>>
        Matrix<P,R> SubMatrix(size_t i, size_t j) {
            Matrix<P,R> ret = CreateMatrix<P,R>(P,R);
            for (size_t r = 0; r < P; ++r) {
                for (size_t c = 0; c < R; ++c) {
                    if (r+i >= this->m || c+j >= this->n) {
                        ret(r,c) = 0;
                    }
                    else {
                        ret(r,c) = this->data[(r+i)*this->n+c+j];
                    }
                }
            }
            return ret;
        }
        Matrix<Dynamic,Dynamic> SubMatrix(size_t i, size_t j, size_t size_m, size_t size_n) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix<Dynamic,Dynamic>(size_m, size_n);
            for (size_t r = 0; r < size_m; ++r) {
                for (size_t c = 0; c < size_n; ++c) {
                    if (r+i >= this->m || c+j >= this->n) {
                        ret(r,c) = 0;
                    }
                    else {
                        ret(r,c) = this->data[(r+i)*this->n+c+j];
                    }
                }
            }
            return ret;
        }

        float Get(size_t r, size_t c) {
            return this->data[r*this->n+c];
        }

        Vector<N> GetRow(size_t r) {
            Vector<N> ret = CreateVector<N>(this->n, 0);
            for (size_t c = 0; c < this->n; ++c) {
                ret[c] = this->data[r*this->n+c];
            }
            return ret;
        }

        Vector<M> GetColumn(size_t c) {
            Vector<M> ret = CreateVector<M>(this->m, 0);
            for (size_t r = 0; r < this->m; ++r) {
                ret[r] = this->data[r*this->n+c];
            }
            return ret;
        }

        template <typename = std::enable_if_t<(N!=Dynamic)>>
        void SetRow(size_t r, float v[N]) {
            for (size_t c = 0; c < this->n; ++c) {
                this->data[r*this->n+c] = v[c];
            }
        }

        void SetRow(size_t r, Vector<N> v) {
            if (this->n != v.Size()) {
                Error("Tried to set a "+std::to_string(this->m)+"x"+std::to_string(this->n)+" matrix's row to a vector of length "+std::to_string(v.Size()));
                return;
            }
            for (size_t c = 0; c < this->n; ++c) {
                this->data[r*this->n+c] = v[c];
            }
        }

        template <typename = std::enable_if_t<(M!=Dynamic)>>
        void SetColumn(size_t c, float v[M]) {
            for (size_t r = 0; r < this->m; ++r) {
                this->data[r*this->n+c] = v[r];
            }
        }

        void SetColumn(size_t c, Vector<M> v) {
            if (this->n != v.Size()) {
                Error("Tried to set a "+std::to_string(this->m)+"x"+std::to_string(this->n)+" matrix's column to a vector of length "+std::to_string(v.Size()));
                return;
            }
            for (size_t r = 0; r < this->m; ++r) {
                this->data[r*this->n+c] = v[r];
            }
        }

        Matrix<(M==0 ? 0 : M-1),N> RemoveRow(size_t i) {
            Matrix<(M==0 ? 0 : M-1),N> ret = CreateMatrix<(M==0 ? 0 : M-1),N>(this->m-1, this->n, 0);
            size_t actualR = 0;
            for (size_t r = 0; r < this->m; ++r) {
                if (r == i) {
                    continue;
                }

                for (size_t c = 0; c < this->n; ++c) {
                    ret(actualR, c) = this->data[r*this->n+c];
                }
                actualR++;
            }
            return ret;
        }

        Matrix<M,(N==0 ? 0 : N-1)> RemoveColumn(size_t i) {
            Matrix<M,(N==0 ? 0 : N-1)> ret = CreateMatrix<M,(N==0 ? 0 : N-1)>(this->m, this->n-1, 0);
            for (size_t r = 0; r < this->m; ++r) {
                size_t actualC = 0;
                for (size_t c = 0; c < this->n; ++c) {
                    if (c == i) {
                        continue;
                    }

                    ret(r, actualC) = this->data[r*this->n+c];
                    actualC++;
                }
            }
            return ret;
        }

        Matrix<(M==0 ? 0 : M-1),(N==0 ? 0 : N-1)> RemoveRowAndColumn(size_t i, size_t j) {
            Matrix<(M==0 ? 0 : M-1),N> ret1 = RemoveRow(i);
            Matrix<(M==0 ? 0 : M-1),(N==0 ? 0 : N-1)> ret2 = ret1.RemoveColumn(j);
            return ret2;
        }

        Matrix<M,N> RowReduce() {
            Matrix<M,N> ret(*this);

            size_t h = 0, k = 0;
            while (h < this->m && k < this->n) {
                // Find the k-th pivot.
                size_t i_max = h;
                float max = -1;
                for (size_t i = h; i < this->m; ++i) {
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
                    for (size_t i = h+1; i < this->m; ++i) {
                        float f = ret(i, k) / ret(h, k);
                        ret(i, k) = 0;

                        // Ri -> Ri - Rh*f
                        for (size_t j = k+1; j < this->n; ++j) {
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
            for (size_t r = 0; r < this->m; ++r) {
                if (lead >= this->n) {
                    return ret;
                }

                // Find the pivot.
                size_t i = r;
                while (ret(i, lead) == 0) {
                    i += 1;
                    if (i == this->m) {
                        i = r;
                        lead += 1;
                        if (lead == this->n) {
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
                for (size_t i = 0; i < this->m; ++i) {
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
            for (size_t c = 0; c < this->n; c++) {
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
            Matrix<(M!=Dynamic&&N!=Dynamic ? M+N : Dynamic),N> augmented = Matrix<(M!=Dynamic&&N!=Dynamic ? M+N : Dynamic),N>::RowAugmented(*this, (N==Dynamic ? Matrix<N,N>::Identity(this->n) : Matrix<N,N>::Identity()));
            augmented = augmented.RCEF();
            Matrix<Dynamic,Dynamic> B = augmented.SubMatrix(0,0,this->m,this->n);
            Matrix<Dynamic,Dynamic> C = augmented.SubMatrix(this->m,0,this->n,this->n);

            std::vector<Vector<N>> basis;
            for (size_t c = 0; c < this->n; c++) {
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

        template <typename = std::enable_if_t<(M!=Dynamic&&N!=Dynamic)>>
        static Matrix<M,N> Zero() {
            return CreateMatrix<M,N>(M,N,0);
        }
        template <typename = std::enable_if_t<(M==Dynamic&&N==Dynamic)>>
        static Matrix<Dynamic,Dynamic> Zero(size_t size_m, size_t size_n) {
            return CreateMatrix<Dynamic,Dynamic>(size_m, size_n, 0);
        }
        template <typename = std::enable_if_t<((M==Dynamic&&N!=Dynamic)||(M!=Dynamic&&N==Dynamic))>>
        static Matrix<M,N> Zero(size_t size) {
            if (M == Dynamic)
                return CreateMatrix<M,N>(size, N, 0);
            else
                return CreateMatrix<M,N>(M, size, 0);
        }

        template <typename = std::enable_if_t<(M!=Dynamic&&N!=Dynamic)>>
        static Matrix<M,N> One() {
            return CreateMatrix<M,N>(M,N,1);
        }
        template <typename = std::enable_if_t<(M==Dynamic&&N==Dynamic)>>
        static Matrix<Dynamic,Dynamic> One(size_t size_m, size_t size_n) {
            return CreateMatrix<Dynamic,Dynamic>(size_m, size_n, 1);
        }
        template <typename = std::enable_if_t<((M==Dynamic&&N!=Dynamic)||(M!=Dynamic&&N==Dynamic))>>
        static Matrix<M,N> One(size_t size) {
            if (M == Dynamic)
                return CreateMatrix<M,N>(size, N, 1);
            else
                return CreateMatrix<M,N>(M, size, 1);
        }

        template <typename = std::enable_if_t<((M!=Dynamic&&N!=Dynamic)&&M==N)>>
        static Matrix<N,N> BackwardIdentity() {
            Matrix<N,N> ret = CreateMatrix<N,N>(N,N,0);
            for (size_t r = 0; r < N; ++r) {
                ret(N-r-1,r) = 1;
            }
            return ret;
        }
        static Matrix<Dynamic,Dynamic> BackwardIdentity(size_t size) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix<Dynamic,Dynamic>(size,size,0);
            for (size_t r = 0; r < size; ++r) {
                ret(size-r-1,r) = 1;
            }
            return ret;
        }

        template <typename = std::enable_if_t<((N!=Dynamic))>>
        static Matrix<M,N> Vandermonde(Vector<M> alpha) {
            Matrix<M,N> ret = CreateMatrix<M,N>(alpha.Size(),N,0);
            for (size_t r = 0; r < alpha.Size(); ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r, c) = std::pow(alpha[r], c);
                }
            }
            return ret;
        }
        template <typename = std::enable_if_t<((N==Dynamic))>>
        static Matrix<M,N> Vandermonde(Vector<M> alpha, size_t size) {
            Matrix<M,N> ret = CreateMatrix<M,N>(alpha.Size(),size,0);
            for (size_t r = 0; r < alpha.Size(); ++r) {
                for (size_t c = 0; c < size; ++c) {
                    ret(r, c) = std::pow(alpha[r], c);
                }
            }
            return ret;
        }

        template <typename = std::enable_if_t<((M!=Dynamic&&N!=Dynamic)&&M==N)>>
        static Matrix<N,N> LowerShift() {
            return Matrix<N,N>::SubDiag(Vector<N-1>::One());
        }
        static Matrix<Dynamic,Dynamic> LowerShift(size_t size) {
            return Matrix<Dynamic,Dynamic>::SubDiag(Vector<Dynamic>::One(size-1));
        }

        template <typename = std::enable_if_t<((M!=Dynamic&&N!=Dynamic)&&M==N)>>
        static Matrix<N,N> UpperShift() {
            return Matrix<N,N>::SuperDiag(Vector<N-1>::One());
        }
        static Matrix<Dynamic,Dynamic> UpperShift(size_t size) {
            return Matrix<Dynamic,Dynamic>::SuperDiag(Vector<Dynamic>::One(size-1));
        }

        bool IsSymmetric() {
            return (*this == Transpose());
        }

        bool IsOrthogonal() {
            return (IsSquare() && Transpose()*(*this) == (*this)*Transpose() && Transpose()*(*this) == Matrix<Dynamic,Dynamic>::Identity(this->n));
        }

        bool IsLowerTrianglar() {
            if (!IsSquare()) {
                return false;
            }

            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = r+1; c < this->n; ++c) {
                    if (Get(r,c) != 0) {
                        return false;
                    }
                }
            }

            return true;
        }

        bool IsUpperTrianglar() {
            if (!IsSquare()) {
                return false;
            }

            for (size_t c = 0; c < this->n; ++c) {
                for (size_t r = c+1; r < this->m; ++r) {
                    if (Get(r,c) != 0) {
                        return false;
                    }
                }
            }

            return true;
        }

        bool IsDiagonal() {
            return (IsUpperTrianglar() && IsLowerTrianglar());
        }

        bool IsTriangular() {
            return (IsUpperTrianglar() || IsLowerTrianglar());
        }

        bool IsPermutation() {
            if (!IsSquare()) {
                return false;
            }

            std::vector<size_t> permutation;
            for (size_t c = 0; c < this->n; ++c) {
                Vector<N> col = GetColumn(c);
                for (size_t i = 0; i < this->n; ++i) {
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

        static Matrix<1,N> RowVector(Vector<N> v) {
            Matrix<1,N> ret = CreateMatrix<N>(1,v.Size(),0);
            for (size_t c = 0; c < ret.NumColumns(); ++c) {
                ret(0,c) = v[c];
            }
            return ret;
        }

        static Matrix<M,1> ColumnVector(Vector<M> v) {
            Matrix<M,1> ret = CreateMatrix<M>(v.Size(),1,0);
            for (size_t r = 0; r < ret.NumRows(); ++r) {
                ret(r,0) = v[r];
            }
            return ret;
        }

        Vector<N> Solve(Vector<N> b) {
            Vector<N> x;
            Matrix<N,N> L, U, P;

            if (IsLowerTrianglar()) {
                for (size_t r = 0; r < this->n; ++r) {
                    float rhs = b[r];
                    for (size_t c = 0; c < r; ++c) {
                        rhs -= x[c]*Get(r,c);
                    }
                    x[r] = rhs / Get(r,r);
                }
                return x;
            }
            else if (IsUpperTrianglar()) {
                for (size_t r = this->n-1; r >= 0; r--) {
                    float rhs = b[r];
                    for (size_t c = this->n-1; c > r; c--) {
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

        template <typename = std::enable_if_t<(M==N)>>
        Vector<N> CramersRule(Vector<N> b) {
            Vector<N> x = CreateVector<N>(b.Size(), 0);
            float detA = Determinant();
            if (detA == 0) {
                Error("Unable to perform Cramers rule.");
                return x;
            }

            for (size_t i = 0; i < this->n; ++i) {
                Matrix<N,N> Ai(*this);
                Ai.SetColumn(i, b);
                float detAi = Ai.Determinant();
                x[i] = detAi / detA;
            }

            return x;
        }

        template <typename = std::enable_if_t<(M==N)>>
        void QRDecomp(Matrix<N,N> & Q, Matrix<N,N> & R) {
            std::vector<Vector<N>> columns;
            for (size_t c = 0; c < this->n; ++c) {
                columns.push_back(GetColumn(c));
            }

            std::vector<Vector<N>> e = Vector<N>::GramSchmidt(columns);
            for (size_t c = 0; c < this->n; ++c) {
                Q.SetColumn(c, e[c]);
            }

            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    if (c < r) {
                        R(r,c) = 0;
                    }
                    else {
                        R(r,c) = Vector<N>::Dot(e[r], columns[c]);
                    }
                }
            }
        }

        template <typename = std::enable_if_t<(M==N)>>
        bool LUPDecomp(Matrix<N,N> & L, Matrix<N,N> & U, Matrix<N,N> & P) {
            L = *this;

            std::vector<size_t> permutations;
            for (size_t i = 0; i < this->n; ++i) {
                permutations.push_back(i);
            }

            for (size_t i = 0; i < this->n; ++i) {
                float maxA = -1;
                size_t i_max = i;

                for (size_t k = i; k < this->n; ++k) {
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

                for (size_t j = i+1; j < this->n; ++j) {
                    L(j,i) /= L(i,i);

                    for (size_t k = i+1; k < this->n; ++k) {
                        L(j, k) -= L(j,i) * L(i,k);
                    }
                }
            }

            // Create the matrices.
            U = L;
            for (size_t r = 0; r < this->n; ++r) {
                L(r, r) = 1;
                for (size_t c = r+1; c < this->n; ++c) {
                    L(r, c) = 0;
                    U(c, r) = 0;
                }
            }

            for (size_t r = 0; r < this->n; ++r) {
                P.SetRow(r, Vector<N>::Basis(permutations[r]));
            }

            return true;
        }

        template<size_t M1, size_t N1, size_t M2, size_t N2>
        static typename std::enable_if<(M1!=Dynamic||N1!=Dynamic||M2!=Dynamic||N2!=Dynamic),Matrix<M1+M2,N1+N2>>::type BlockMatrix(Matrix<M1,N1> A, Matrix<M1,N2> B, Matrix<M2,N1> C, Matrix<M2,N2> D) {
            Matrix<M1+M2,N1+N2> ret;
            for (size_t r = 0; r < M1+M2; ++r) {
                for (size_t c = 0; c < N1+N2; ++c) {
                    if (r < M1 && c < N1) {
                        ret(r,c) = A(r,c);
                    }
                    else if (r < M1 && c >= N1) {
                        ret(r,c) = B(r,c-N1);
                    }
                    else if (r >= M1 && c < N1) {
                        ret(r,c) = C(r-M1,c);
                    }
                    else if (r >= M1 && c >= N1) {
                        ret(r,c) = D(r-M1,c-N1);
                    }
                }
            }
            return ret;
        }

        template<size_t M1, size_t N1, size_t M2, size_t N2>
        static typename std::enable_if<(M1==Dynamic||N1==Dynamic||M2==Dynamic||N2==Dynamic), Matrix<Dynamic,Dynamic>>::type BlockMatrix(Matrix<M1,N1> A, Matrix<M1,N2> B, Matrix<M2,N1> C, Matrix<M2,N2> D) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix(A.NumRows()+C.NumRows(), A.NumColumns()+B.NumColumns(), 0);
            for (size_t r = 0; r < A.NumRows()+C.NumRows(); ++r) {
                for (size_t c = 0; c < A.NumColumns()+B.NumColumns(); ++c) {
                    if (r < A.NumRows() && c < A.NumColumns()) {
                        ret(r,c) = A(r,c);
                    }
                    else if (r < A.NumRows() && c >= A.NumColumns()) {
                        ret(r,c) = B(r,c-A.NumColumns());
                    }
                    else if (r >= A.NumRows() && c < A.NumColumns()) {
                        ret(r,c) = C(r-A.NumRows(),c);
                    }
                    else if (r >= A.NumRows() && c >= A.NumColumns()) {
                        ret(r,c) = D(r-A.NumRows(),c-A.NumColumns());
                    }
                }
            }
            return ret;
        }

        template<size_t M, size_t N>
        static std::conditional_t<(M==Dynamic||N==Dynamic),Matrix<Dynamic,Dynamic>,Matrix<M+1,N+1>> BlockMatrix(float a, Matrix<1,N> b, Matrix<M,1> c, Matrix<M,N> d) {
            return std::conditional_t<(M==Dynamic||N==Dynamic),Matrix<Dynamic,Dynamic>,Matrix<M+1,N+1>>::BlockMatrix(Matrix<1,1>(a), b, c, d);
        }

        template<size_t M, size_t N>
        static std::conditional_t<(M==Dynamic||N==Dynamic),Matrix<Dynamic,Dynamic>,Matrix<M+1,N+1>> BlockMatrix(float a, Vector<N> b, Vector<M> c, Matrix<M,N> d) {
            return std::conditional_t<(M==Dynamic||N==Dynamic),Matrix<Dynamic,Dynamic>,Matrix<M+1,N+1>>::BlockMatrix(Matrix<1,1>(a), Matrix<1,N>::RowVector(b), Matrix<M,1>::ColumnVector(c), d);
        }

        template<size_t M, size_t N>
        static std::conditional_t<(M==Dynamic||N==Dynamic),Matrix<Dynamic,Dynamic>,Matrix<M+1,N+1>> BlockMatrix(Matrix<M,N> a, Matrix<M,1> b, Matrix<1,N> c, float d) {
            return std::conditional_t<(M==Dynamic||N==Dynamic),Matrix<Dynamic,Dynamic>,Matrix<M+1,N+1>>::BlockMatrix(a, b, c, Matrix<1,1>(d));
        }

        template<size_t M, size_t N>
        static std::conditional_t<(M==Dynamic||N==Dynamic),Matrix<Dynamic,Dynamic>,Matrix<M+1,N+1>> BlockMatrix(Matrix<M,N> a, Vector<M> b, Vector<N> c, float d) {
            return std::conditional_t<(M==Dynamic||N==Dynamic),Matrix<Dynamic,Dynamic>,Matrix<M+1,N+1>>::BlockMatrix(a, Matrix<M,1>::ColumnVector(b), Matrix<1,N>::RowVector(c), Matrix<1,1>(d));
        }

        template<size_t P, size_t Q>
        static Matrix<P,Q> OuterProduct(Vector<P> u, Vector<Q> v) {
            return (Matrix<1,P>(u).Transpose())*(Matrix<1,Q>(v));
        }

        template<size_t M1, size_t N1, size_t M2, size_t N2>
        static typename std::enable_if<(M1!=Dynamic||N1!=Dynamic||M2!=Dynamic||N2!=Dynamic),Matrix<M1+M2,N1+N2>>::type DirectSum(Matrix<M1,N1> a, Matrix<M2,N2> b) {
            return Matrix<M1+M2,N1+N2>::BlockMatrix(a, Matrix<M1,N2>::Zero(), Matrix<M2,N1>::Zero(), b);
        }
        template<size_t M1, size_t N1, size_t M2, size_t N2>
        static typename std::enable_if<(M1==Dynamic||N1==Dynamic||M2==Dynamic||N2==Dynamic),Matrix<Dynamic,Dynamic>>::type DirectSum(Matrix<M1,N1> a, Matrix<M2,N2> b) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix(a.NumRows()+b.NumRows(), a.NumColumns()+b.NumColumns());
            return Matrix<M1+M2,N1+N2>::BlockMatrix(a, Matrix<Dynamic,Dynamic>::Zero(a.NumRows(), b.NumColumns()),
                Matrix<Dynamic,Dynamic>::Zero(b.NumRows(),a.NumColumns()), b);
        }

        template<size_t M1, size_t N1, size_t M2, size_t N2>
        static typename std::enable_if<(M1!=Dynamic||N1!=Dynamic||M2!=Dynamic||N2!=Dynamic),Matrix<M1*M2,N1*N2>>::type KroneckerProduct(Matrix<M1,N1> a, Matrix<M2,N2> b) {
            Matrix<M1*M2,N1*N2> ret;
            for (size_t r = 0; r < M1*M2; ++r) {
                for (size_t c = 0; c < N1*N2; ++c) {
                    ret(r,c) = a(r/M2,c/M2)*b(r%M2,c%N2);
                }
            }

            return ret;
        }
        template<size_t M1, size_t N1, size_t M2, size_t N2>
        static typename std::enable_if<(M1==Dynamic||N1==Dynamic||M2==Dynamic||N2==Dynamic),Matrix<Dynamic,Dynamic>>::type KroneckerProduct(Matrix<M1,N1> a, Matrix<M2,N2> b) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix(a.NumRows()*b.NumRows(), a.NumColumns()*b.NumColumns());
            for (size_t r = 0; r < a.NumRows()*b.NumRows(); ++r) {
                for (size_t c = 0; c < a.NumColumns()*b.NumColumns(); ++c) {
                    ret(r,c) = a(r/b.NumRows(),c/b.NumRows())*b(r%b.NumRows(),c%b.NumColumns());
                }
            }

            return ret;
        }

        template<size_t P, size_t Q>
        static typename std::enable_if<(P!=Dynamic&&Q!=Dynamic),Matrix<P*Q,P*Q>>::type KroneckerSum(Matrix<P,P> a, Matrix<Q,Q> b) {
            Matrix<P*Q,P*Q> ret = KroneckerProduct(a, Matrix<Q,Q>::Identity());
            ret += KroneckerProduct(Matrix<P,P>::Identity(), b);

            return ret;
        }
        template<size_t P, size_t Q>
        static typename std::enable_if<(P==Dynamic||Q==Dynamic),Matrix<Dynamic,Dynamic>>::type KroneckerSum(Matrix<P,P> a, Matrix<Q,Q> b) {
            Matrix<Dynamic,Dynamic> ret = KroneckerProduct(a, Matrix<Dynamic,Dynamic>::Identity(b.NumRows()));
            ret += KroneckerProduct(Matrix<Dynamic,Dynamic>::Identity(a.NumRows()), b);

            return ret;
        }

        static Matrix<M, N> EntrywiseProduct(Matrix<M,N> a, Matrix<M,N> b) {
            Matrix<M,N> ret = CreateMatrix<M,N>(a.NumRows(), a.NumColumns(), 0);
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    ret(r,c) = a(r,c) * b(r,c);
                }
            }

            return ret;
        }

        template<size_t P, size_t Q>
        static void Grid(Vector<P> x, Vector<Q> y, Matrix<Q,P> & X, Matrix<Q,P> & Y) {
            X = CreateMatrix<Q,P>(y.Size(), x.Size());
            Y = CreateMatrix<Q,P>(y.Size(), x.Size());
            for (size_t r = 0; r < y.Size(); ++r) {
                X.SetRow(r, x);
            }
            for (size_t c = 0; c < x.Size(); ++c) {
                Y.SetColumn(c, y);
            }
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
        static typename std::enable_if<(M!=Dynamic&&P!=Dynamic&&Q!=Dynamic),Matrix<M,P+Q>>::type Augmented(Matrix<M,P> a, Matrix<M,Q> b) {
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
        static typename std::enable_if<(M==Dynamic||P==Dynamic||Q==Dynamic),Matrix<Dynamic,Dynamic>>::type Augmented(Matrix<M,P> a, Matrix<M,Q> b) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix(a.NumRows(), a.NumColumns()+b.NumColumns());
            for (size_t r = 0; r < a.NumRows(); ++r) {
                for (size_t c = 0; c < a.NumColumns()+b.NumColumns(); ++c) {
                    if (c < a.NumColumns())
                        ret(r,c) = a(r,c);
                    else
                        ret(r,c) = b(r,c-a.NumColumns());
                }
            }
            return ret;
        }

        template<size_t P, size_t Q>
        static typename std::enable_if<(N!=Dynamic&&P!=Dynamic&&Q!=Dynamic),Matrix<P+Q,N>>::type RowAugmented(Matrix<P,N> a, Matrix<Q,N> b) {
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
        template<size_t P, size_t Q>
        static typename std::enable_if<(N==Dynamic||P==Dynamic||Q==Dynamic),Matrix<Dynamic,Dynamic>>::type RowAugmented(Matrix<P,N> a, Matrix<Q,N> b) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix(a.NumRows()+b.NumRows(),a.NumColumns());
            for (size_t r = 0; r < a.NumRows()+b.NumRows(); ++r) {
                for (size_t c = 0; c < a.NumColumns(); ++c) {
                    if (r < a.NumRows())
                        ret(r,c) = a(r,c);
                    else
                        ret(r,c) = b(r-a.NumRows(),c);
                }
            }
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n), Matrix<N,N>>::type Pow(Matrix<N,N> a, int p) {
            if (p == 0) {
                return (N == Dynamic ? Matrix<N,N>::Identity(this->n) : Matrix<N,N>::Identity());
            }
            else if (p == 1) {
                return a;
            }
            else if (a.IsDiagonal()) {
                Matrix<N,N> ret;
                for (size_t i = 0; i < N; ++i) {
                    ret(i,i) = std::pow(a(i,i), p);
                }
                return ret;
            }

            Matrix<N,N> ret(a);
            for (size_t i = 1; i < std::abs(p); ++i) {
                ret *= a;
            }

            if (p < 0) {
                return ret.Inverse();
            }
            return ret;
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<(m == n), Matrix<N,N>>::type Exp(Matrix<N,N> a, size_t num_terms = 10) {
            if (a.IsDiagonal()) {
                Matrix<N,N> ret = CreateMatrix<N,N>(a.NumRows(), a.NumRows());
                for (size_t i = 0; i < N; ++i) {
                    ret(i,i) = std::exp(a(i,i));
                }
                return ret;
            }

            float factorial = 1;
            Matrix<N,N> ret = (N == Dynamic ? Matrix<N,N>::Identity(this->n) : Matrix<N,N>::Identity());
            for (size_t k = 1; k < num_terms; ++k) {
                factorial *= k;
                ret += Matrix<N,N>::Pow(a, k) / factorial;
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
        template<typename = std::enable_if<(M==N&&N!=Dynamic)>>
        static Matrix<N,N> Identity() {
            return Matrix<N,N>::Diag(Vector<N>(1));
        }
        static Matrix<Dynamic,Dynamic> Identity(size_t size) {
            return Matrix<Dynamic,Dynamic>::Diag(Vector<Dynamic>(size, 1));
        }

        template<size_t m = M, size_t n = N>
        static typename std::enable_if<m == n, Matrix<N,N>>::type Diag(const Vector<N>& vals) {
            Matrix<N,N> ret = CreateMatrix<N,N>(vals.Size(), vals.Size(), 0);
            for (size_t r = 0; r < vals.Size(); ++r) {
                for (size_t c = 0; c < vals.Size(); ++c) {
                    ret(r,c)  = (r == c ? vals[r] : 0);
                }
            }
            return ret;
        }

        template<typename = std::enable_if<(M==N&&N!=Dynamic)>>
        static Matrix<N,N> SuperDiag(const Vector<N-1>& vals) {
            Matrix<N,N> ret(0);
            for (size_t r = 0; r < vals.Size(); ++r) {
                ret(r,r+1) = vals[r];
            }
            return ret;
        }
        template<typename = std::enable_if<(N==Dynamic)>>
        static Matrix<Dynamic,Dynamic> SuperDiag(const Vector<Dynamic>& vals) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix(vals.Size()+1, vals.Size()+1, 0);
            for (size_t r = 0; r < vals.Size(); ++r) {
                ret(r,r+1) = vals[r];
            }
            return ret;
        }

        template<typename = std::enable_if<(M==N&&N!=Dynamic)>>
        static Matrix<N,N> SubDiag(const Vector<N-1>& vals) {
            Matrix<N,N> ret(0);
            for (size_t r = 0; r < vals.Size(); ++r) {
                ret(r+1,r) = vals[r];
            }
            return ret;
        }
        template<typename = std::enable_if<(N==Dynamic)>>
        static Matrix<Dynamic,Dynamic> SubDiag(const Vector<Dynamic>& vals) {
            Matrix<Dynamic,Dynamic> ret = CreateMatrix(vals.Size()+1, vals.Size()+1, 0);
            for (size_t r = 0; r < vals.Size(); ++r) {
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

        template<typename = std::enable_if<(M==N||M==Dynamic||N==Dynamic)>>
        float Trace() {
            if (!IsSquare()) {
                Error("Cannot take the trace of a non-square matrix.");
                return 0;
            }
            float sum = 0;
            for (size_t i = 0; i < N; ++i) {
                sum += this->data[i*N+i];
            }
            return sum;
        }

        template<typename = std::enable_if<(M==N||M==Dynamic||N==Dynamic)>>
        bool IsSingular() {
            return (Determinant() == 0);
        }

        // template<typename = std::enable_if<(M==1&&N==1)>>
        // float Determinant() { return data[0]; }
        // template<typename = std::enable_if<((M==Dynamic&&N==1)||(M==1&&N=Dynamic))>>
        // float Determinant() {
        //     if (!IsSquare()) {
        //         Error("Cannot take the determinant of a non-square matrix.");
        //         return 0;
        //     }
        //     return data[0];
        // }
        //
        // template<typename = std::enable_if<(M==2&&N==2)>>
        // float Determinant() { return data[0]*data[3] - data[1]*data[2]; }
        // template<typename = std::enable_if<((M==Dynamic&&N==2)||(M==2&&N=Dynamic))>>
        // float Determinant() {
        //     if (!IsSquare()) {
        //         Error("Cannot take the determinant of a non-square matrix.");
        //         return 0;
        //     }
        //     return data[0]*data[3] - data[1]*data[2];
        // }

        template<typename = std::enable_if<(M==N||M==Dynamic||N==Dynamic)>>
        float Determinant() {
            if (!IsSquare()) {
                Error("Cannot take the determinant of a non-square matrix.");
                return 0;
            }
            if (this->m == 1 && this->n == 1) {
                return data[0];
            }
            else if (this->m == 2 && this->n == 2) {
                return data[0]*data[3] - data[1]*data[2];
            }
            else if (IsTriangular()) {
                float det = 1;
                for (size_t i = 0; i < this->n; ++i) {
                    det *= Get(i, i);
                }
                return det;
            }

            float det = 0;
            for (size_t i = 0; i < this->n; ++i) {
                det += data[i] * Cofactor(0, i);
            }
            return det;
        }

        template<typename = std::enable_if<(M==N||M==Dynamic||N==Dynamic)>>
        float Minor(size_t i, size_t j) {
            return RemoveRowAndColumn(i, j).Determinant();
        }

        template<typename = std::enable_if<(M==N||M==Dynamic||N==Dynamic)>>
        float Cofactor(size_t i, size_t j) {
            return std::pow(-1.0f, i+j) * Minor(i, j);
        }

        template<typename = std::enable_if<(M==N||M==Dynamic||N==Dynamic)>>
        Matrix<N,N> Adjugate() {
            Matrix<N,N> ret = CreateMatrix<N,N>(this->n, this->n, 0);
            if (!IsSquare()) {
                Error("Cannot compute the adjugate of a non-square matrix.");
                return ret;
            }
            for (size_t r = 0; r < this->n; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    ret(r,c) = Cofactor(r, c);
                }
            }
            return ret.Transpose();
        }

        template<typename = std::enable_if<(M==N||M==Dynamic||N==Dynamic)>>
        Matrix<N,N> Inverse() {
            if (!IsSquare()) {
                throw "Cannot compute the inverse of a non-square matrix.";
            }
            float det = Determinant();
            if (det == 0) {
                throw "Inverse of singular matrix.";
            }

            return (1/det)*Adjugate();
        }

        // operators
        float& operator() (size_t r, size_t c) { return this->data[r*this->n+c]; }
        const float& operator() (size_t r, size_t c) const { return this->data[r*this->n+c]; }

        Matrix<M,N>& operator=(const Matrix<M,N>& other) {
            if (this != &other) {
                for (size_t r = 0; r < this->m; ++r) {
                    for (size_t c = 0; c < this->n; ++c) {
                        this->data[r*this->n+c] = other(r,c);
                    }
                }
            }
            return *this;
        }
        friend bool operator==(const Matrix<M,N>& lhs, const Matrix<M,N>& rhs) {
            if (lhs.NumRows() != rhs.NumRows() || lhs.NumColumns() != rhs.NumColumns()) {
                return false;
            }
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) != rhs(r,c))
                        return false;
                }
            }
            return true;
        }
        friend bool operator!=(const Matrix<M,N>& lhs, const Matrix<M,N>& rhs) { return !(lhs == rhs); }
        friend Matrix<M,N> operator==(const Matrix<M,N>& lhs, float rhs) {
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) == rhs)
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator!=(const Matrix<M,N>& lhs, float rhs) {
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) != rhs)
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator>(const Matrix<M,N>& lhs, float rhs) {
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) > rhs)
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator>(const Matrix<M,N>& lhs, const Matrix<M,N>& rhs) {
            if (lhs.NumRows() != rhs.NumRows() || lhs.NumColumns() != rhs.NumColumns()) {
                throw "Failed to compare two matrices.";
            }
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) > rhs(r,c))
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator>=(const Matrix<M,N>& lhs, float rhs) {
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) >= rhs)
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator>=(const Matrix<M,N>& lhs, const Matrix<M,N>& rhs) {
            if (lhs.NumRows() != rhs.NumRows() || lhs.NumColumns() != rhs.NumColumns()) {
                throw "Failed to compare two matrices.";
            }
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) >= rhs(r,c))
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator<(const Matrix<M,N>& lhs, float rhs) {
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) < rhs)
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator<(const Matrix<M,N>& lhs, const Matrix<M,N>& rhs) {
            if (lhs.NumRows() != rhs.NumRows() || lhs.NumColumns() != rhs.NumColumns()) {
                throw "Failed to compare two matrices.";
            }
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) < rhs(r,c))
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator<=(const Matrix<M,N>& lhs, float rhs) {
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) <= rhs)
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        friend Matrix<M,N> operator<=(const Matrix<M,N>& lhs, const Matrix<M,N>& rhs) {
            if (lhs.NumRows() != rhs.NumRows() || lhs.NumColumns() != rhs.NumColumns()) {
                throw "Failed to compare two matrices.";
            }
            Matrix<M,N> ret = CreateMatrix<M,N>(lhs.NumRows(), lhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < lhs.NumColumns(); ++c) {
                    if (lhs(r,c) <= rhs(r,c))
                        ret(r,c) = 1;
                }
            }
            return ret;
        }
        Matrix<M,N>& operator+=(const Matrix<M,N>& rhs) {
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    this->data[r*this->n+c] += rhs(r,c);
                }
            }
            return *this;
        }
        friend Matrix<M,N> operator+(Matrix<M,N> lhs, const Matrix<M,N>& rhs) { return lhs += rhs; }
        Matrix<M,N>& operator-=(const Matrix<M,N>& rhs) {
            for (size_t r = 0; r < this->m; ++r) {
                for (size_t c = 0; c < this->n; ++c) {
                    this->data[r*this->n+c] -= rhs(r,c);
                }
            }
            return *this;
        }
        friend Matrix<M,N> operator-(const Matrix<M,N>& rhs) { return -1.0f * rhs; }
        friend Matrix<M,N> operator-(Matrix<M,N> lhs, const Matrix<M,N>& rhs) { return lhs -= rhs; }
        Matrix<M,N>& operator*=(const float& rhs) {
            for (size_t i = 0; i < this->m*this->n; ++i) {
                this->data[i] *= rhs;
            }
            return *this;
        }
        friend Matrix<M,N> operator*(Matrix<M,N> lhs, const float& rhs) { return lhs *= rhs; }
        friend Matrix<M,N> operator*(const float& lhs, Matrix<M,N> rhs) { return rhs *= lhs; }
        Matrix<M,N>& operator/=(const float& rhs) {
            for (size_t i = 0; i < this->m*this->n; ++i) {
                this->data[i] /= rhs;
            }
            return *this;
        }
        friend Matrix<M,N> operator/(Matrix<M,N> lhs, const float& rhs) { return lhs /= rhs; }
        template<size_t P, size_t Q, typename = std::enable_if<(N==P||N==Dynamic||P==Dynamic)>>
        friend Matrix<M,Q> operator*(Matrix<M,N> lhs, Matrix<P,Q> rhs) {
            if (lhs.NumColumns() != rhs.NumRows()) {
                throw "Invalid sizes for matrix-matrix multiplication.";
            }
            Matrix<M,Q> ret = CreateMatrix<M,Q>(lhs.NumRows(), rhs.NumColumns());
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
                for (size_t c = 0; c < rhs.NumColumns(); ++c) {
                    Vector<N> row = lhs.GetRow(r);
                    Vector<P> column = rhs.GetColumn(c);
                    ret(r,c) = Vector<N>::Dot(row, column);
                }
            }
            return ret;
        }
        template<size_t P, typename = std::enable_if<(N==P||N==Dynamic||P==Dynamic)>>
        friend Vector<M> operator*(Matrix<M,N> lhs, Vector<P> rhs) {
            if (lhs.NumColumns() != rhs.Size()) {
                throw "Invalid sizes for matrix-vector multiplication.";
            }
            Vector<M> ret = CreateVector<M>(lhs.NumRows(), 0);
            for (size_t r = 0; r < lhs.NumRows(); ++r) {
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
        size_t m, n;
        float * data;
    private:
        template <size_t P, size_t Q>
        static typename std::enable_if<(P==Dynamic&&Q==Dynamic), Matrix<P,Q>>::type CreateMatrix(size_t t_m, size_t t_n, float value = 0) {
            Matrix<P,Q> ret(t_m, t_n, value);
            return ret;
        }
        template <size_t P, size_t Q>
        static typename std::enable_if<((P==Dynamic&&Q!=Dynamic)||(P!=Dynamic&&Q==Dynamic)), Matrix<P,Q>>::type CreateMatrix(size_t t_m, size_t t_n, float value = 0) {
            Matrix<P,Q> ret((P==Dynamic ? t_m : t_n), value);
            return ret;
        }
        template <size_t P, size_t Q>
        static typename std::enable_if<(P!=Dynamic&&Q!=Dynamic), Matrix<P,Q>>::type CreateMatrix(size_t t_m, size_t t_n, float value = 0) {
            Matrix<P,Q> ret(value);
            return ret;
        }

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

    using Matrix2 = Matrix<2,2>;
    using Matrix3 = Matrix<3,3>;
    using Matrix4 = Matrix<4,4>;
}
