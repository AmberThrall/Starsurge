#pragma once
#include <string>
#include "Vector.h"

namespace Starsurge {
    static long double PI = 3.141592653589793238462643383279502884L;
    static long double PHI = 1.61803398875L;

    float Infinity();
    float NaN();
    bool IsInfinite(float x);
    bool IsNaN(float x);
    bool IsZero(float v, float eps = 0.00001);
    bool IsSame(float a, float b, float eps = 0.00001);
    float Radians(float deg);
    float Degrees(float rad);
    float Ceil(float x);
    float Floor(float x);
    float Round(float x);
    float Trunc(float x);
    float Fract(float x);
    float Max(float a, float b);
    template <typename...Ts>
    float Max(float a, float b, Ts&&... ts) {
        float ret = Max(a,b);
        return Max(ret, std::forward<Ts>(ts)...);
    }
    float Min(float a, float b);
    template <typename...Ts>
    float Min(float a, float b, Ts&&... ts) {
        float ret = Min(a,b);
        return Min(ret, std::forward<Ts>(ts)...);
    }
    float Clamp(float v, float min, float max);
    float Wrap(float v, float min, float max);
    float Sign(float x);
    float Sqrt(float x);
    float InverseSqrt(float x);
    float Mod(float x, float modulus);
    float Abs(float x);
    float Sin(float rad);
    float Cos(float rad);
    float Tan(float rad);
    float Csc(float rad);
    float Sec(float rad);
    float Cot(float rad);
    float ASin(float x);
    float ACos(float x);
    float ATan(float x);
    float ATan2(float y, float x);
    float ACsc(float x);
    float ASec(float x);
    float ACot(float x);
    float ACot2(float x, float y);
    float Sinh(float x);
    float Cosh(float x);
    float Tanh(float x);
    float Csch(float x);
    float Sech(float x);
    float Coth(float x);
    float ASinh(float x);
    float ACosh(float x);
    float ATanh(float x);
    float ACsch(float x);
    float ASech(float x);
    float ACoth(float x);
    float Exp(float x);
    float Exp2(float x);
    float Log(float x);
    float Log(float x, float base);
    float Log10(float x);
    float Log2(float x);
    float Pow(float x, float exp);
    float Step(float edge, float x);
    float Smoothstep(float edge0, float edge1, float x);
    float Smootherstep(float edge0, float edge1, float x);
    float InverseSmoothstep(float y);
    float Lerp(float start, float end, float t);
    float Spline(float p0, float m0, float p1, float m1, float t);
    std::string ToString(bool v);
    std::string ToString(int v);
    std::string ToString(long v);
    std::string ToString(long long v);
    std::string ToString(unsigned int v);
    std::string ToString(unsigned long v);
    std::string ToString(unsigned long long v);
    std::string ToString(float v, unsigned int ndigits = 6, bool scientific = false);
    std::string ToString(double v, unsigned int ndigits = 6, bool scientific = false);
    std::string ToString(long double v, unsigned int ndigits = 6, bool scientific = false);
}
