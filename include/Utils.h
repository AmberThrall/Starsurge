#pragma once
#include <vector>
#include <string>
#include <regex>

namespace Starsurge {
    static long double PI = 3.141592653589793238462643383279502884L; /* pi */
    static char* WHITESPACE = " \t\n\r\f\v"; // Why does static instead of const fix this?

    std::vector<std::string> Explode(std::string const & str, char delim, bool include_empty = true);
    std::vector<std::string> ExplodeWhitespace(std::string const & str, bool include_empty = true);
    std::string LTrim(std::string const str);
    std::string RTrim(std::string const str);
    std::string Trim(std::string const str);

    std::string Replace(std::string const str, std::string regex, const std::string & to);
    void PrintCode(std::string code);

    float Radians(float deg);
    float Degrees(float rad);

    struct Match {
        unsigned int position;
        std::vector<std::string> matches;
        std::string prefix;
        std::string suffix;
    };

    std::vector<Match> FindSubstrings(std::string str, std::string regex);
    Match FindFirstSubstrings(std::string str, std::string regex);
    Match FindLastSubstrings(std::string str, std::string regex);

    template<typename T>
    bool ElemOf(const T array[], const unsigned int arraySize, const T query) {
        for (unsigned int i = 0; i < arraySize; ++i) {
            if (std::strcmp(typeid(T).name(), "char const *") == 0) {
                if (std::strcmp(array[i], query) == 0) {
                    return true;
                }
            }
            else {
                if (array[i] == query) {
                    return true;
                }
            }
        }
        return false;
    }

    template<typename T>
    bool ElemOf(const std::vector<T> v, T query) {
        for (unsigned int i = 0; i < v.size(); ++i) {
            if (v[i] == query) {
                return true;
            }
        }
        return false;
    }
}
