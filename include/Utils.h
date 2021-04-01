#pragma once
#include <vector>
#include <string>
#include <regex>
#include "Numerics.h"

namespace Starsurge {
    static char* WHITESPACE = " \t\n\r\f\v"; // Why does static instead of const fix this?

    std::vector<std::string> Explode(std::string const & str, char delim, bool include_empty = true);
    std::vector<std::string> Explode(std::string const & str, std::string delim, bool include_empty = true);
    std::vector<std::string> ExplodeWhitespace(std::string const & str, bool include_empty = true);
    std::string LTrim(std::string const str);
    std::string RTrim(std::string const str);
    std::string Trim(std::string const str);

    std::string Replace(std::string const str, std::string regex, const std::string & to);
    void PrintCode(std::string code);

    // Select various elements from an tuple
    template <typename Tuple, size_t... Idx>
    auto Select(Tuple&& tuple, std::index_sequence<Idx...>) {
        return std::make_tuple(std::get<Idx>(std::forward<Tuple>(tuple))...);
    }

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

    template<typename T>
    int Index(const std::vector<T> v, T query) {
        for (unsigned int i = 0; i < v.size(); ++i) {
            if (v[i] == query) {
                return i;
            }
        }
        return -1;
    }
}
