#include "../include/Utils.h"
#include <sstream>
#include <utility>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include "../include/Logging.h"

std::vector<std::string> Starsurge::Explode(std::string const & str, char delim, bool include_empty) {
    //https://stackoverflow.com/a/12967010
    std::vector<std::string> res;
    std::istringstream iss(str);

    for (std::string token; std::getline(iss, token, delim); ) {
        if (!token.empty() || include_empty) {
            res.push_back(std::move(token));
        }
    }

    return res;
}

std::vector<std::string> Starsurge::ExplodeWhitespace(std::string const & str, bool include_empty) {
    std::vector<std::string> res;

    std::size_t lastIndex = 0;
    std::size_t index = str.find_first_of(WHITESPACE, 0);
    while (index != std::string::npos) {
        std::string substr = str.substr(lastIndex, index-lastIndex);
        if (!substr.empty() || include_empty) {
            res.push_back(substr);
        }
        lastIndex = index+1;
        index = str.find_first_of(WHITESPACE, lastIndex);
    }
    std::string substr = str.substr(lastIndex);
    if (!substr.empty() || include_empty) {
        res.push_back(substr);
    }

    return res;
}

std::string Starsurge::LTrim(std::string const str) {
    //https://stackoverflow.com/a/25385766
    std::string res = str;
    res.erase(0, res.find_first_not_of(WHITESPACE));
    return res;
}

std::string Starsurge::RTrim(std::string const str) {
    //https://stackoverflow.com/a/25385766
    std::string res = str;
    res.erase(res.find_last_not_of(WHITESPACE)+1);
    return res;
}

std::string Starsurge::Trim(std::string const str) {
    //https://stackoverflow.com/a/25385766
    return LTrim(RTrim(str));
}
