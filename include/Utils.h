#pragma once
#include <vector>
#include <string>

namespace Starsurge {
    static char* WHITESPACE = " \t\n\r\f\v"; // Why does static instead of const fix this?

    std::vector<std::string> Explode(std::string const & str, char delim, bool include_empty = true);
    std::vector<std::string> ExplodeWhitespace(std::string const & str, bool include_empty = true);
    std::string LTrim(std::string const str);
    std::string RTrim(std::string const str);
    std::string Trim(std::string const str);

    template<typename T>
    bool ElemOf(const T array[], const unsigned int arraySize, T query) {
        for (unsigned int i = 0; i < arraySize; ++i) {
            if (std::strcmp(typeid(T).name(), "char const *")==0) {
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
        return ElemOf<T>(v.data(), v.size(), query);
    }
}
