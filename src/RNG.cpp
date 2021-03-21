#include "../include/RNG.h"
#include "../include/Utils.h"

Starsurge::RNG& Starsurge::RNG::Inst() {
    static RNG instance;
    return instance;
}

void Starsurge::RNG::Seed(unsigned int t_seed) {
    this->gen.seed(t_seed);
    this->seed = t_seed;
}

unsigned int Starsurge::RNG::GetSeed() {
    return this->seed;
}

float Starsurge::RNG::Uniform(float low, float high) {
    high = std::max(low, high);
    if (low == high) {
        return low;
    }
    std::uniform_real_distribution<> distr(low, high);
    return distr(this->gen);
}

bool Starsurge::RNG::Bernoulli(float p) {
    std::bernoulli_distribution distr(p);
    return distr(this->gen);
}

int Starsurge::RNG::Binomial(float p, unsigned int n) {
    std::binomial_distribution<> distr(n, p);
    return distr(this->gen);
}

int Starsurge::RNG::NegativeBinomial(float p, unsigned int r) {
    std::negative_binomial_distribution<> distr(r, p);
    return distr(this->gen);
}

int Starsurge::RNG::Geometric(float p) {
    std::geometric_distribution<> distr(p);
    return distr(this->gen);
}

int Starsurge::RNG::Poisson(float mean) {
    std::poisson_distribution<> distr(mean);
    return distr(this->gen);
}

float Starsurge::RNG::Exponential(float lambda) {
    std::exponential_distribution<> distr(lambda);
    return distr(this->gen);
}

float Starsurge::RNG::Gamma(float alpha, float beta) {
    std::gamma_distribution<> distr(alpha, beta);
    return distr(this->gen);
}

float Starsurge::RNG::Weibull(float a, float b) {
    std::weibull_distribution<> distr(a, b);
    return distr(this->gen);
}

float Starsurge::RNG::ExtremeValue(float a, float b) {
    std::extreme_value_distribution<> distr(a, b);
    return distr(this->gen);
}

float Starsurge::RNG::Normal(float mu, float sigma) {
    std::normal_distribution<> distr{mu, sigma};
    return distr(this->gen);
}

float Starsurge::RNG::LogNormal(float m, float s) {
    std::lognormal_distribution<> distr(m, s);
    return distr(this->gen);
}

float Starsurge::RNG::ChiSquared(float n) {
    std::chi_squared_distribution<float> distr(n);
    return distr(this->gen);
}

float Starsurge::RNG::Cauchy(float a, float b) {
    std::cauchy_distribution<float> distr(a, b);
    return distr(this->gen);
}

float Starsurge::RNG::F(float m, float n) {
    std::fisher_f_distribution<float> distr(m, n);
    return distr(this->gen);
}

float Starsurge::RNG::StudentT(float n) {
    std::student_t_distribution<float> distr(n);
    return distr(this->gen);
}


unsigned int Starsurge::RNG::UInteger() {
    return this->gen();
}

int Starsurge::RNG::Integer() {
    return UInteger() - this->gen.max() / 2;
}

int Starsurge::RNG::Integer(int low, int high) {
    std::uniform_int_distribution<> distr(low, high);
    return distr(this->gen);
}
