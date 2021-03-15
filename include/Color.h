#pragma once
#include "Vector.h"

namespace Starsurge {
    class Color : public Vector4 {
    public:
        Color(float all = 0);
        Color(float r, float g, float b);
        Color(float r, float g, float b, float a);

        static Color FromHex(std::string hex);
        static Color FromHSL(float hue, float saturation, float lightness);
        static Color FromHSV(float hue, float saturation, float value);
        static Color FromCYMK(float cyan, float magenta, float yellow, float black);

        std::string ToHex();

        Color ToOpenGLFormat();
    };

    namespace Colors {
        const Color CLEAR = Color(0, 0, 0, 0);
        const Color INDIAN_RED = Color(205, 92, 92);
        const Color LIGHT_CORAL = Color(240, 128, 128);
        const Color SALMON = Color(250, 128, 114);
        const Color DARK_SALMON = Color(233, 150, 122);
        const Color LIGHT_SALMON = Color(255, 160, 122);
        const Color CRIMSON = Color(220, 20, 60);
        const Color RED = Color(255, 0, 0);
        const Color FIRE_BRICK = Color(178, 34, 34);
        const Color DARK_RED = Color(139, 0, 0);
        const Color PINK = Color(255, 192, 203);
        const Color LIGHT_PINK = Color(255, 182, 193);
        const Color HOT_PINK = Color(255, 105, 180);
        const Color DEEP_PINK = Color(255, 20, 147);
        const Color MEDIUM_VIOLET_RED = Color(199, 21, 133);
        const Color PALE_VIOLET_RED = Color(219, 112, 147);
        const Color CORAL = Color(255, 127, 80);
        const Color TOMATO = Color(255, 99, 71);
        const Color ORANGE_RED = Color(255, 69, 0);
        const Color DARK_ORANGE = Color(255, 140, 0);
        const Color ORANGE = Color(255, 165, 0);
        const Color GOLD = Color(255, 215, 0);
        const Color YELLOW = Color(255, 255, 0);
        const Color LIGHT_YELLOW = Color(255, 255, 224);
        const Color LEMON_CHIFFON = Color(255, 250, 205);
        const Color LIGHT_GOLDENROD_YELLOW = Color(250, 250, 210);
        const Color PAPAYA_WHIP = Color(255, 239, 213);
        const Color MOCCASIN = Color(255, 228, 181);
        const Color PEACH_PUFF = Color(255, 218, 185);
        const Color PALE_GOLDENROD = Color(238, 232, 170);
        const Color KHAKI = Color(240, 230, 140);
        const Color DARK_KHAKI = Color(189, 183, 107);
        const Color LAVENDER = Color(230, 230, 250);
        const Color THISTLE = Color(216, 191, 216);
        const Color PLUM = Color(221, 160, 221);
        const Color VIOLET = Color(238, 130, 238);
        const Color ORCHID = Color(218, 112, 214);
        const Color FUCHSIA = Color(255, 0, 255);
        const Color MAGENTA = Color(255, 0, 255);
        const Color MEDIUM_ORCHID = Color(186, 85, 211);
        const Color MEDIUM_PURPLE = Color(147, 112, 219);
        const Color REBECCA_PURPLE = Color(102, 51, 153);
        const Color BLUE_VIOLET = Color(138, 43, 226);
        const Color DARK_VIOLET = Color(148, 0, 211);
        const Color DARK_ORCHID = Color(153, 50, 204);
        const Color DARK_MAGENTA = Color(139, 0, 139);
        const Color PURPLE = Color(128, 0, 128);
        const Color INDIGO = Color(75, 0, 130);
        const Color SLATE_BLUE = Color(106, 90, 205);
        const Color DARK_SLATE_BLUE = Color(72, 61, 139);
        const Color MEDIUM_SLATE_BLUE = Color(123, 104, 238);
        const Color GREEN_YELLOW = Color(173, 255, 47);
        const Color CHARTREUSE = Color(127, 255, 0);
        const Color LAWN_GREEN = Color(124, 252, 0);
        const Color LIME = Color(0, 255, 0);
        const Color LIME_GREEN = Color(50, 205, 50);
        const Color PALE_GREEN = Color(152, 251, 152);
        const Color LIGHT_GREEN = Color(144, 238, 144);
        const Color MEDIUM_SPRING_GREEN = Color(0, 250, 154);
        const Color SPRING_GREEN = Color(0, 255, 127);
        const Color MEDIUM_SEA_GREEN = Color(60, 179, 113);
        const Color SEA_GREEN = Color(46, 139, 87);
        const Color FOREST_GREEN = Color(34, 139, 34);
        const Color GREEN = Color(0, 128, 0);
        const Color DARK_GREEN = Color(0, 100, 0);
        const Color YELLOW_GREEN = Color(154, 205, 50);
        const Color OLIVE_DRAB = Color(107, 142, 35);
        const Color OLIVE = Color(128, 128, 0);
        const Color DARK_OLIVE_GREEN = Color(85, 107, 47);
        const Color MEDIUM_AQUAMARINE = Color(102, 205, 170);
        const Color DARK_SEA_GREEN = Color(143, 188, 139);
        const Color LIGHT_SEA_GREEN = Color(32, 178, 170);
        const Color DARK_CYAN = Color(0, 139, 139);
        const Color TEAL = Color(0, 128, 128);
        const Color AQUA = Color(0, 255, 255);
        const Color CYAN = Color(0, 255, 255);
        const Color LIGHT_CYAN = Color(224, 255, 255);
        const Color PALE_TURQUOISE = Color(175, 238, 238);
        const Color AQUAMARINE = Color(127, 255, 212);
        const Color TURQUOISE = Color(64, 224, 208);
        const Color MEDIUM_TURQUOISE = Color(72, 209, 204);
        const Color DARK_TURQUOISE = Color(0, 206, 209);
        const Color CADET_BLUE = Color(95, 158, 160);
        const Color STEEL_BLUE = Color(70, 130, 180);
        const Color LIGHT_STEEL_BLUE = Color(176, 196, 222);
        const Color POWDER_BLUE = Color(176, 224, 230);
        const Color LIGHT_BLUE = Color(173, 216, 230);
        const Color SKY_BLUE = Color(135, 206, 235);
        const Color LIGHT_SKY_BLUE = Color(135, 206, 250);
        const Color DEEP_SKY_BLUE = Color(0, 191, 255);
        const Color DODGER_BLUE = Color(30, 144, 255);
        const Color CORNFLOWER_BLUE = Color(100, 149, 237);
        const Color ROYAL_BLUE = Color(65, 105, 225);
        const Color BLUE = Color(0, 0, 255);
        const Color MEDIUM_BLUE = Color(0, 0, 205);
        const Color DARK_BLUE = Color(0, 0, 139);
        const Color NAVY = Color(0, 0, 128);
        const Color MIDNIGHT_BLUE = Color(25, 25, 112);
        const Color CORNSILK = Color(255, 248, 220);
        const Color BLANCHED_ALMOND = Color(255, 235, 205);
        const Color BISQUE = Color(255, 228, 196);
        const Color NAVAJO_WHITE = Color(255, 222, 173);
        const Color WHEAT = Color(245, 222, 179);
        const Color BURLY_WOOD = Color(222, 184, 135);
        const Color TAN = Color(210, 180, 140);
        const Color ROSY_BROWN = Color(188, 143, 143);
        const Color SANDY_BROWN = Color(244, 164, 96);
        const Color GOLDENROD = Color(218, 165, 32);
        const Color DARK_GOLDENROD = Color(184, 134, 11);
        const Color PERU = Color(205, 133, 63);
        const Color CHOCOLATE = Color(210, 105, 30);
        const Color SADDLE_BROWN = Color(139, 69, 19);
        const Color SIENNA = Color(160, 82, 45);
        const Color BROWN = Color(165, 42, 42);
        const Color MAROON = Color(128, 0, 0);
        const Color WHITE = Color(255, 255, 255);
        const Color SNOW = Color(255, 250, 250);
        const Color HONEY_DEW = Color(240, 255, 240);
        const Color MINT_CREAM = Color(245, 255, 250);
        const Color AZURE = Color(240, 255, 255);
        const Color ALICE_BLUE = Color(240, 248, 255);
        const Color GHOST_WHITE = Color(248, 248, 255);
        const Color WHITE_SMOKE = Color(245, 245, 245);
        const Color SEA_SHELL = Color(255, 245, 238);
        const Color BEIGE = Color(245, 245, 220);
        const Color OLD_LACE = Color(253, 245, 230);
        const Color FLORAL_WHITE = Color(255, 250, 240);
        const Color IVORY = Color(255, 255, 240);
        const Color ANTIQUE_WHITE = Color(250, 235, 215);
        const Color LINEN = Color(250, 240, 230);
        const Color LAVENDER_BLUSH = Color(255, 240, 245);
        const Color MISTY_ROSE = Color(255, 228, 225);
        const Color GAINSBORO = Color(220, 220, 220);
        const Color LIGHT_GRAY = Color(211, 211, 211);
        const Color SILVER = Color(192, 192, 192);
        const Color DARK_GRAY = Color(169, 169, 169);
        const Color GRAY = Color(128, 128, 128);
        const Color DIM_GRAY = Color(105, 105, 105);
        const Color LIGHT_SLATE_GRAY = Color(119, 136, 153);
        const Color SLATE_GRAY = Color(112, 128, 144);
        const Color DARK_SLATE_GRAY = Color(47, 79, 79);
        const Color BLACK = Color(0, 0, 0);
    }
}
