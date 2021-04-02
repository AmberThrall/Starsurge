#include "../include/Numerics.h"

float Starsurge::Infinity() {
    return std::numeric_limits<float>::infinity();
}
float Starsurge::NaN() {
    return std::numeric_limits<float>::quiet_NaN();
}

bool Starsurge::IsInfinite(float x) {
    return std::isinf(x);
}
bool Starsurge::IsNaN(float x) {
    return std::isnan(x);
}
bool Starsurge::IsZero(float v, float eps) {
    return (Abs(v) < eps);
}
bool Starsurge::IsSame(float a, float b, float eps) {
    return IsZero(a-b, eps);
}

float Starsurge::Radians(float deg) {
    return deg*(PI / 180.0f);
}

float Starsurge::Degrees(float rad) {
    return rad*(180.0f / PI);
}

float Starsurge::Ceil(float x) {
    return std::ceil(x);
}
float Starsurge::Floor(float x) {
    return std::floor(x);
}
float Starsurge::Round(float x) {
    return std::round(x);
}
float Starsurge::Trunc(float x) {
    return std::trunc(x);
}
float Starsurge::Fract(float x) {
    return x - Floor(x);
}

float Starsurge::Max(float a, float b) {
    if (a > b)
        return a;
    return b;
}

float Starsurge::Min(float a, float b) {
    if (a < b)
        return a;
    return b;
}

float Starsurge::Clamp(float v, float min, float max) {
    return Max(Min(v, max), min);
}

float Starsurge::Wrap(float v, float min, float max) {
    while (v < min) {
        v += std::abs(max-min);
    }
    while (v > max) {
        v -= std::abs(max-min);
    }
    return v;
}

float Starsurge::Sign(float x) {
    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 0;
}

float Starsurge::Sqrt(float x) {
    return std::sqrt(x);
}
float Starsurge::InverseSqrt(float x) {
    return 1.0/Sqrt(x);
}
float Starsurge::Mod(float x, float modulus) {
    return std::fmod(x, modulus);
}
float Starsurge::Abs(float x) {
    if (x < 0) {
        return -1*x;
    }
    return x;
}
float Starsurge::Sin(float rad) {
    return std::sin(rad);
}
float Starsurge::Cos(float rad) {
    return std::cos(rad);
}
float Starsurge::Tan(float rad) {
    return std::tan(rad);
}
float Starsurge::Csc(float rad) {
    return 1/Sin(rad);
}
float Starsurge::Sec(float rad) {
    return 1/Cos(rad);
}
float Starsurge::Cot(float rad) {
    return 1/Tan(rad);
}
float Starsurge::ASin(float x) {
    return std::asin(x);
}
float Starsurge::ACos(float x) {
    return std::acos(x);
}
float Starsurge::ATan(float x) {
    return std::atan(x);
}
float Starsurge::ATan2(float y, float x) {
    return std::atan2(y, x);
}
float Starsurge::ACsc(float x) {
    return ASin(1/x);
}
float Starsurge::ASec(float x) {
    return ACos(1/x);
}
float Starsurge::ACot(float x) {
    return ATan(1/x);
}
float Starsurge::ACot2(float x, float y) {
    return ATan2(y, x);
}
float Starsurge::Sinh(float x) {
    return std::sinh(x);
}
float Starsurge::Cosh(float x) {
    return std::cosh(x);
}
float Starsurge::Tanh(float x) {
    return std::tanh(x);
}
float Starsurge::Csch(float x) {
    return 1 / Sinh(x);
}
float Starsurge::Sech(float x) {
    return 1 / Cosh(x);
}
float Starsurge::Coth(float x) {
    return 1 / Tanh(x);
}
float Starsurge::ASinh(float x) {
    return std::asinh(x);
}
float Starsurge::ACosh(float x) {
    return std::acosh(x);
}
float Starsurge::ATanh(float x) {
    return std::atanh(x);
}
float Starsurge::ACsch(float x) {
    return ASinh(1/x);
}
float Starsurge::ASech(float x) {
    return ACosh(1/x);
}
float Starsurge::ACoth(float x) {
    return ATanh(1/x);
}
float Starsurge::Exp(float x) {
    return std::exp(x);
}
float Starsurge::Exp2(float x) {
    return std::exp2(x);
}
float Starsurge::Log(float x) {
    return std::log(x);
}
float Starsurge::Log(float x, float base) {
    return Log(x) / Log(base);
}
float Starsurge::Log10(float x) {
    return std::log10(x);
}
float Starsurge::Log2(float x) {
    return std::log2(x);
}
float Starsurge::Pow(float x, float exp) {
    return std::pow(x, exp);
}

float Starsurge::Step(float edge, float x) {
    if (x < edge) {
        return 0;
    }
    return 1;
}
float Starsurge::Smoothstep(float edge0, float edge1, float x) {
    x = Clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return x * x * (3 - 2 * x); // 3x^2-2x^3
}
float Starsurge::Smootherstep(float edge0, float edge1, float x) {
    x = Clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return x * x * x * (x * (x * 6 - 15) + 10); // 6x^5-15x^4+10x^3
}
float Starsurge::InverseSmoothstep(float y) {
    return 0.5 - Sin(ASin(1.0 - 2.0*y) / 3.0);
}

float Starsurge::Lerp(float start, float end, float t) {
    t = Clamp(t, 0, 1);
    return start + t*(end-start);
}
float Starsurge::Spline(float p0, float m0, float p1, float m1, float t) {
    t = Clamp(t, 0, 1);
    return (1+2*t)*(1-t)*(1-t)*p0 + t*(1-t)*(1-t)*m0 + t*t*(3-2*t)*p1+t*t*(t-1)*m1;
}

std::string Starsurge::ToString(bool v) {
    return (v == true) ? std::string("true") : std::string("false");
}
std::string Starsurge::ToString(int v) { return std::to_string(v); }
std::string Starsurge::ToString(long v) { return std::to_string(v); }
std::string Starsurge::ToString(long long v) { return std::to_string(v); }
std::string Starsurge::ToString(unsigned int v) { return std::to_string(v); }
std::string Starsurge::ToString(unsigned long v) { return std::to_string(v); }
std::string Starsurge::ToString(unsigned long long v) { return std::to_string(v); }
std::string Starsurge::ToString(float v, unsigned int ndigits, bool scientific) {
    if (std::isnan(v)) return "NaN";
    if (std::isinf(v)) return "Inf";
    if ((std::round(v) == v || ndigits == 0) && !scientific) return ToString((int)v);

    char * buffer = new char[ndigits+256];
    sprintf(buffer, (scientific ? "%.*e" : "%.*f"), ndigits, v);
    return std::string(buffer);
}
std::string Starsurge::ToString(double v, unsigned int ndigits, bool scientific) {
    if (std::isnan(v)) return "NaN";
    if (std::isinf(v)) return "Inf";
    if ((std::round(v) == v || ndigits == 0) && !scientific) return ToString((int)v);

    char * buffer = new char[ndigits+256];
    sprintf(buffer, (scientific ? "%.*e" : "%.*f"), ndigits, v);
    return std::string(buffer);
}
std::string Starsurge::ToString(long double v, unsigned int ndigits, bool scientific) {
    if (std::isnan(v)) return "NaN";
    if (std::isinf(v)) return "Inf";
    if ((std::round(v) == v || ndigits == 0) && !scientific) return ToString((int)v);

    char * buffer = new char[ndigits+256];
    sprintf(buffer, (scientific ? "%.*e" : "%.*f"), ndigits, v);
    return std::string(buffer);
}
