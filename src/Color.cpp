#include "../include/Color.h"
#include "../include/Utils.h"

Starsurge::Color::Color(float all){
    Color(all, all, all, 255);
}

Starsurge::Color::Color(float r, float g, float b) {
    this->data[0] = Clamp(r, 0, 255);
    this->data[1] = Clamp(g, 0, 255);
    this->data[2] = Clamp(b, 0, 255);
    this->data[3] = 255;
}

Starsurge::Color::Color(float r, float g, float b, float a) {
    this->data[0] = Clamp(r, 0, 255);
    this->data[1] = Clamp(g, 0, 255);
    this->data[2] = Clamp(b, 0, 255);
    this->data[3] = Clamp(a, 0, 255);
}

Starsurge::Color Starsurge::Color::FromHex(std::string hex) {
    if (hex[0] != '#' || (hex.length() != 7 && hex.length() != 9)) {
        Error("Not a valid hex code.");
        return Colors::CLEAR;
    }
    for (unsigned int i = 1; i < hex.length(); ++i) {
        char c = hex[i];
        if (!((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))) {
            Error("Not a valid hex code.");
            return Colors::CLEAR;
        }
    }

    std::string R = hex.substr(1,2);
    std::string G = hex.substr(3,2);
    std::string B = hex.substr(5,2);

    Color ret;
    ret[0] = (float)std::stoi("0x"+R, 0, 16);
    ret[1] = (float)std::stoi("0x"+G, 0, 16);
    ret[2] = (float)std::stoi("0x"+B, 0, 16);
    ret[3] = 255;

    if (hex.length() == 9) {
        std::string A = hex.substr(7,2);
        ret[3] = (float)std::stoi("0x"+A, 0, 16);
    }
    return ret;
}

std::string Starsurge::Color::ToHex() {
    std::string hex = "#";
    std::string table = "0123456789ABCDEF";
    for (unsigned int i = 0; i < 4; ++i) {
        int value = (int)std::round(this->data[i]);
        int upper = value / 16;
        int lower = value % 16;
        hex += table[upper];
        hex += table[lower];
    }
    return hex;
}

Starsurge::Color Starsurge::Color::FromHSL(float hue, float saturation, float lightness) {
    // https://www.rapidtables.com/convert/color/hsl-to-rgb.html
    hue = Clamp(hue, 0, 360);
    saturation = Clamp(saturation, 0, 1);
    lightness = Clamp(lightness, 0, 1);

    float C = (1-std::abs(2*lightness-1))*saturation;
    float X = C * (1-std::abs(std::fmod(hue / 60, 2) - 1));
    float m = lightness - C/2;
    Vector3 temp;
    if (hue >= 0 && hue < 60) { temp = Vector3(C, X, 0); }
    if (hue >= 60 && hue < 120) { temp = Vector3(X, C, 0); }
    if (hue >= 120 && hue < 180) { temp = Vector3(0, C, X); }
    if (hue >= 180 && hue < 240) { temp = Vector3(0, X, C); }
    if (hue >= 240 && hue < 300) { temp = Vector3(X, 0, C); }
    if (hue >= 300 && hue <= 360) { temp = Vector3(C, 0, X); }

    return Color(((temp[0] + m) * 255), ((temp[1] + m) * 255), ((temp[2] + m) * 255), 255);
}

Starsurge::Color Starsurge::Color::FromHSV(float hue, float saturation, float value) {
    // https://www.rapidtables.com/convert/color/hsv-to-rgb.html
    hue = Clamp(hue, 0, 360);
    saturation = Clamp(saturation, 0, 1);
    value = Clamp(value, 0, 1);

    float C = value * saturation;
    float X = C * (1-std::abs(std::fmod(hue/60, 2) - 1));
    float m = value - C;
    Vector3 temp;
    if (hue >= 0 && hue < 60) { temp = Vector3(C, X, 0); }
    if (hue >= 60 && hue < 120) { temp = Vector3(X, C, 0); }
    if (hue >= 120 && hue < 180) { temp = Vector3(0, C, X); }
    if (hue >= 180 && hue < 240) { temp = Vector3(0, X, C); }
    if (hue >= 240 && hue < 300) { temp = Vector3(X, 0, C); }
    if (hue >= 300 && hue <= 360) { temp = Vector3(C, 0, X); }

    return Color(((temp[0] + m) * 255), ((temp[1] + m) * 255), ((temp[2] + m) * 255), 255);
}

Starsurge::Color Starsurge::Color::FromCYMK(float cyan, float magenta, float yellow, float black) {
    // https://www.rapidtables.com/convert/color/cmyk-to-rgb.html
    cyan = Clamp(cyan, 0, 1);
    magenta = Clamp(magenta, 0, 1);
    yellow = Clamp(yellow, 0, 1);
    black = Clamp(black, 0, 1);

    return Color(255*(1-cyan)*(1-black), 255*(1-magenta)*(1-black), 255*(1-yellow)*(1-black), 255);
}

Starsurge::Color Starsurge::Color::ToOpenGLFormat() {
    Color ret(*this);
    ret[0] /= 255.0;
    ret[1] /= 255.0;
    ret[2] /= 255.0;
    ret[3] /= 255.0;
    return ret;
}
