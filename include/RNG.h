#pragma once
#include <random>
#include "Vector.h"
#include "Matrix.h"

namespace Starsurge {
    class RNG {
    public:
        static RNG& Inst();

        void Seed(unsigned int t_seed);
        unsigned int GetSeed();

        float Uniform(float low = 0, float high = 1);
        bool Bernoulli(float p = 0.5);
        int Binomial(float p = 0.5, unsigned int n = 1);
        int NegativeBinomial(float p = 0.5, unsigned int r = 1);
        int Geometric(float p = 0.5);
        int Poisson(float mean = 1);
        float Exponential(float lambda = 1);
        float Gamma(float alpha = 1, float beta = 1);
        float Weibull(float a = 1, float b = 1);
        float ExtremeValue(float a = 1, float b = 0);
        float Normal(float mu = 0, float sigma = 1);
        float LogNormal(float m = 0, float s = 1);
        float ChiSquared(float n = 1);
        float Cauchy(float a = 0, float b = 1);
        float F(float m = 1, float n = 1);
        float StudentT(float n = 1);
        unsigned int UInteger();
        int Integer();
        int Integer(int low, int high);

        template <typename T>
        T Choice(std::vector<T> choices) {
            int i = Integer(0, choices.size()-1);
            return choices[i];
        }
        template <typename T>
        T Choice(std::vector<T> choices, std::initializer_list<double> probabilities) {
            if (choices.size() != probabilities.size()) {
                throw "choices.size() != probabilities.size()";
            }
            std::discrete_distribution<> d(probabilities);
            int i = d(this->gen);
            return choices[i];
        }

        template<size_t N>
        Vector<N> Uniform(float low = 0, float high = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Uniform(low, high);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Uniform(float low = 0, float high = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Uniform(low, high);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Bernoulli(float p = 0.5) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Bernoulli(p);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Bernoulli(float p = 0.5) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Bernoulli(p);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Binomial(float p = 0.5, unsigned int n = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Binomial(p, n);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Binomial(float p = 0.5, unsigned int n = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Binomial(p, n);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> NegativeBinomial(float p = 0.5, unsigned int r = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = NegativeBinomial(p, r);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> NegativeBinomial(float p = 0.5, unsigned int r = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = NegativeBinomial(p, r);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Geometric(float p = 0.5) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Geometric(p);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Geometric(float p = 0.5) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Geometric(p);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Poisson(float mean = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Poisson(mean);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Poisson(float mean = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Poisson(mean);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Exponential(float lambda = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Exponential(lambda);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Exponential(float lambda = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Exponential(lambda);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Gamma(float alpha = 1, float beta = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Gamma(alpha, beta);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Gamma(float alpha = 1, float beta = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Gamma(alpha, beta);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Weibull(float a = 1, float b = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Weibull(a, b);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Weibull(float a = 1, float b = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Weibull(a, b);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> ExtremeValue(float a = 1, float b = 0) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = ExtremeValue(a, b);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> ExtremeValue(float a = 1, float b = 0) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = ExtremeValue(a, b);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Normal(float mu = 0, float sigma = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Normal(mu, sigma);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Normal(float mu = 0, float sigma = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Normal(mu, sigma);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> LogNormal(float m = 0, float s = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = LogNormal(m, s);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> LogNormal(float m = 0, float s = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = LogNormal(m, s);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> ChiSquared(float n = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = ChiSquared(n);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> ChiSquared(float n = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = ChiSquared(n);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Cauchy(float a = 0, float b = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Cauchy(a, b);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Cauchy(float a = 0, float b = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Cauchy(a, b);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> F(float m = 1, float n = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = F(m, n);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> F(float m = 1, float n = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = F(m, n);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> StudentT(float n = 1) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = StudentT(n);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> StudentT(float n = 1) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = StudentT(n);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> UInteger() {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = UInteger();
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> UInteger() {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = UInteger();
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Integer() {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Integer();
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Integer() {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Integer();
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Integer(int low, int high) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Integer(low, high);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Integer(int low, int high) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Integer(low, high);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Choice(std::vector<float> choices) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Choice(choices);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Choice(std::vector<float> choices) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Choice(choices);
                }
            }
            return ret;
        }

        template<size_t N>
        Vector<N> Choice(std::vector<float> choices, std::initializer_list<double> probabilities) {
            Vector<N> ret;
            for (size_t i = 0; i < N; ++i) {
                ret[i] = Choice(choices, probabilities);
            }
            return ret;
        }
        template<size_t M, size_t N>
        Matrix<M,N> Choice(std::vector<float> choices, std::initializer_list<double> probabilities) {
            Matrix<M,N> ret;
            for (size_t r = 0; r < M; ++r) {
                for (size_t c = 0; c < N; ++c) {
                    ret(r,c) = Choice(choices, probabilities);
                }
            }
            return ret;
        }
    private:
        RNG() {};

        std::mt19937 gen;
        unsigned int seed;
    };
}
