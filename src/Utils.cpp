#include "../include/Utils.h"
#include <sstream>
#include <utility>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <regex>
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


std::vector<std::string> Starsurge::Explode(std::string const & str, std::string delim, bool include_empty) {
    std::vector<std::string> res;
    size_t index = str.find(delim);
    size_t lastIndex = 0;

    while (index != std::string::npos) {
        std::string substr = str.substr(lastIndex, index-lastIndex);
        if (!substr.empty() || include_empty) {
            res.push_back(std::move(substr));
        }
        lastIndex = index+delim.length();
        index = str.find(delim, lastIndex);
    }

    std::string substr = str.substr(lastIndex);
    if (!substr.empty() || include_empty) {
        res.push_back(std::move(substr));
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

std::string Starsurge::Replace(std::string const str, std::string rgx, const std::string & to) {
    std::string res = str;
    res = std::regex_replace(str, std::regex(rgx), to);
    return res;
}

void Starsurge::PrintCode(std::string code) {
    std::vector<std::string> lines = Explode(code, '\n', true);
    std::string whitespace = "";
    float nlines = lines.size();
    while (nlines >= 10) {
        whitespace += " ";
        nlines /= 10.0;
    }

    for (unsigned int i = 0; i < lines.size(); ++i) {
        float curLine = i+1;
        std::string newwhitespace = whitespace;
        while (curLine >= 10) {
            newwhitespace = newwhitespace.substr(1);
            curLine /= 10.0;
        }
        std::cout << (i+1) << newwhitespace << "| " << lines[i] << std::endl;
    }
}

float Starsurge::Radians(float deg) {
    return deg*(PI / 180.0f);
}

float Starsurge::Degrees(float rad) {
    return rad*(180.0f / PI);
}

float Starsurge::Max(float v, float max) {
    if (v > max)
        return max;
    return v;
}

float Starsurge::Min(float v, float min) {
    if (v < min)
        return min;
    return v;
}

float Starsurge::Clamp(float v, float min, float max) {
    return Min(Max(v, max), min);
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

float Starsurge::Lerp(float start, float end, float t) {
    return start + t*(end-start);
}

std::vector<Starsurge::Match> Starsurge::FindSubstrings(std::string str, std::string regex) {
    std::smatch m;
    std::vector<Match> ret;
    while (std::regex_search(str, m, std::regex(regex))) {
        if (m.size() == 0) {
            break;
        }

        Match newMatch;
        newMatch.position = m.position();
        newMatch.prefix = m.prefix();
        newMatch.suffix = m.suffix();
        for (unsigned int i = 0; i < m.size(); ++i) {
            newMatch.matches.push_back(m[i]);
        }
        ret.push_back(newMatch);

        str = m.suffix();
    }
    return ret;
}

Starsurge::Match Starsurge::FindFirstSubstrings(std::string str, std::string regex) {
    std::vector<Match> matches = FindSubstrings(str, regex);
    if (matches.size() == 0) {
        Match newMatch;
        newMatch.position = 0;
        newMatch.prefix = "";
        newMatch.suffix = "";
        return newMatch;
    }
    return matches[0];
}

Starsurge::Match Starsurge::FindLastSubstrings(std::string str, std::string regex) {
    std::vector<Match> matches = FindSubstrings(str, regex);
    if (matches.size() == 0) {
        Match newMatch;
        newMatch.position = 0;
        newMatch.prefix = "";
        newMatch.suffix = "";
        return newMatch;
    }
    return matches[matches.size()-1];
}
