#pragma once

#include <map>
#include <string>

namespace Starsurge {
    namespace GLSL {
        enum TokenType {
            TOKEN_EOF, TOKEN_KEYWORD, TOKEN_TYPE, TOKEN_QUALIFIER, TOKEN_IDENTIFIER, TOKEN_INT, TOKEN_FLOAT, TOKEN_SEMICOLON,
            TOKEN_SCOPE_BEGIN, TOKEN_SCOPE_END, TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET, TOKEN_OP_INC,
            TOKEN_OP_DEC, TOKEN_OP_PLUS, TOKEN_OP_MINUS, TOKEN_OP_BANG, TOKEN_OP_STAR, TOKEN_OP_SLASH, TOKEN_OP_LT, TOKEN_OP_GT, TOKEN_OP_LTEQ,
            TOKEN_OP_GTEQ, TOKEN_OP_EQUALS, TOKEN_OP_NOT_EQUALS, TOKEN_OP_AND, TOKEN_OP_XOR, TOKEN_OP_OR, TOKEN_OP_QUESTION, TOKEN_OP_ASSIGN,
            TOKEN_OP_PLUS_EQUAL, TOKEN_OP_MINUS_EQUAL, TOKEN_OP_STAR_EQUAL, TOKEN_OP_SLASH_EQUAL, TOKEN_OP_MOD, TOKEN_OP_LEFT, TOKEN_OP_RIGHT,
            TOKEN_OP_BITWISE_AND, TOKEN_OP_BITWISE_XOR, TOKEN_OP_BITWISE_OR, TOKEN_OP_MOD_EQUAL, TOKEN_OP_LEFT_EQUAL, TOKEN_OP_RIGHT_EQUAL,
            TOKEN_OP_BITWISE_AND_EQUAL, TOKEN_OP_BITWISE_XOR_EQUAL, TOKEN_OP_BITWISE_OR_EQUAL, TOKEN_OP_DOT, TOKEN_OP_COLON, TOKEN_OP_COMMA,
            TOKEN_OP_TILDE, TOKEN_LAYOUT, TOKEN_BREAK, TOKEN_CONTINUE, TOKEN_DO, TOKEN_FOR, TOKEN_WHILE, TOKEN_SWITCH, TOKEN_CASE, TOKEN_DEFAULT,
            TOKEN_IF, TOKEN_ELSE, TOKEN_TRUE, TOKEN_FALSE, TOKEN_DISCARD, TOKEN_RETURN, TOKEN_PRECISION, TOKEN_STRUCT
        };

        static const char* TYPES[] = {
            // Basics
            "bool", "int", "uint", "float", "void",
            // Vectors
            "bvec2", "bvec3", "bvec4", "ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4", "vec2", "vec3", "vec4",
            // Matrices
            "mat2x2", "mat2x3", "mat2x4", "mat3x2", "mat3x3", "mat3x4", "mat4x2", "mat4x3", "mat4x4", "mat2", "mat3", "mat4",
            // Samplers
            "sampler1D", "sampler2D", "sampler3D", "samplerCube", "sampler2DRect", "sampler1DArray", "sampler2DArray", "samplerBuffer",
            "sampler2DMS", "sampler2DMSArray",
            "isampler1D", "isampler2D", "isampler3D", "isamplerCube", "isampler2DRect", "isampler1DArray", "isampler2DArray", "isamplerBuffer",
            "isampler2DMS", "isampler2DMSArray",
            "usampler1D", "usampler2D", "usampler3D", "usamplerCube", "usampler2DRect", "usampler1DArray", "usampler2DArray",
            "usamplerBuffer", "usampler2DMS", "usampler2DMSArray",
            "sampler1DShadow", "sampler2DShadow", "sampler2DRectShadow", "sampler1DArrayShadow", "sampler2DArrayShadow",
            // Custom type for colors equaling vec4
            "color"
        };
        static unsigned int TYPES_COUNT = 65;

        static const char* QUALIFIERS_STORAGE[] = {
            "const", "in", "out", "varying", "attribute", "uniform", "buffer", "shared"
        };
        static unsigned int QUALIFIERS_STORAGE_COUNT = 7;
        static const char* QUALIFIERS_MISC[] = {
            "centroid", "sample", "patch"
        };
        static unsigned int QUALIFIERS_MISC_COUNT = 3;
        static const char* QUALIFIERS_INTERP[] = {
            "smooth", "flat", "noperspective"
        };
        static unsigned int QUALIFIERS_INTERP_COUNT = 3;
        static const char* QUALIFIERS_PREC[] = {
            "highp", "mediump", "lowp"
        };
        static unsigned int QUALIFIERS_PREC_COUNT = 3;
        static const char* QUALIFIERS_MEMORY[] = {
            "coherent", "volatile", "restrict", "readonly", "writeonly"
        };
        static unsigned int QUALIFIERS_MEMORY_COUNT = 3;
        static const char* QUALIFIERS_INVARIANT = "invariant";

        //https://ptgmedia.pearsoncmg.com/images/9780321552624/downloads/0321552628_AppI.pdf
        // static const char * BUILTIN_STRUCTS[] = {}
        // static unsigned int BUILTIN_STRUCTS_COUNT = 0;
        // static const char * BUILTIN_VARIABLES[] = {}
        // static unsigned int BUILTIN_VARIABLES_COUNT = 0;
        static const char * BUILTIN_FUNCTIONS[] = {
            // Trig. functions
            "radians", "degrees", "sin", "cos", "tan", "asin", "atan", "sinh", "cosh", "tanh", "asinh",
            // Exponential functions
            "pow", "exp", "log", "exp2", "log2", "sqrt", "inversesqrt",
            // Common functions
            "abs", "sign", "floor", "trunc", "round", "roundEven", "ceil", "fract", "mod", "modf", "min", "max",
            "clamp", "mix", "step", "smoothstep", "isnan", "isinf", "floatBitsToInt", "floatBitsToUint",
            "intBitsToFloat", "uintBitsToFloat",
            // Geometric functions
            "length", "distance", "dot", "cross", "normalize", "faceforward", "reflect", "refract",
            // Matrix functions
            "matrixCompMult", "outerProduct", "transpose", "determinant", "inverse",
            // Vector Relational Functions
            "lessThan", "lessThanEqual", "greaterThan", "greaterThanEqual", "equal", "notEqual", "any", "all", "not",
            // Texture Lookup Functions
            "textureSize", "texture", "textureProj", "textureLod", "textureOffset", "texelFetch", "texelFetchOffset",
            "textureProjOffset", "textureLodOffset", "textureProjLod", "textureProjLodOffset", "textureGrad",
            "textureGradOffset", "textureProjOffset", "textureProjGradOffset",
            // Fragment Processing Functions
            "dFdx", "dFdy", "fwidth",
            // Noise functions
            "noise1", "noise2", "noise3", "noise4",
            // Geometry Shader functions
            "EmitVertex", "EndPrimitive"
        };
        static unsigned int BUILTIN_FUNCTIONS_COUNT = 86;

        static unsigned int MAX_OPERATOR_PRECEDENCE = 14;
        static std::map<TokenType, int> OPERATOR_PRECEDENCE = {
            // { TOKEN_LEFT_PAREN, 17 },
            // { TOKEN_RIGHT_PAREN, 17 },
            // { TOKEN_LEFT_BRACKET, 16 },
            // { TOKEN_RIGHT_BRACKET, 16 },
            // { TOKEN_OP_DOT, 16 },
            //{ TOKEN_OP_INC, 15 }, // Note: 16 when used as post-fix
            //{ TOKEN_OP_DEC, 15 }, // Note: 16 when used as post-fix
            // { TOKEN_OP_TILDE, 15 },
            // { TOKEN_OP_BANG, 15 },
            { TOKEN_OP_STAR, 14 },
            { TOKEN_OP_SLASH, 14 },
            { TOKEN_OP_MOD, 14 },
            { TOKEN_OP_PLUS, 13 }, // Note: 15 when unary
            { TOKEN_OP_MINUS, 13 }, // Note: 15 when unary
            { TOKEN_OP_LEFT, 12 },
            { TOKEN_OP_RIGHT, 12 },
            { TOKEN_OP_LT, 11 },
            { TOKEN_OP_GT, 11 },
            { TOKEN_OP_LTEQ, 11 },
            { TOKEN_OP_GTEQ, 11 },
            { TOKEN_OP_EQUALS, 10 },
            { TOKEN_OP_NOT_EQUALS, 10 },
            { TOKEN_OP_BITWISE_AND, 9 },
            { TOKEN_OP_BITWISE_XOR, 8 },
            { TOKEN_OP_BITWISE_OR, 7 },
            { TOKEN_OP_AND, 6 },
            { TOKEN_OP_XOR, 5 },
            { TOKEN_OP_OR, 4 },
            { TOKEN_OP_QUESTION, 3 },
            { TOKEN_OP_COLON, 3 },
            { TOKEN_OP_ASSIGN, 2 },
            { TOKEN_OP_PLUS_EQUAL, 2 },
            { TOKEN_OP_MINUS_EQUAL, 2 },
            { TOKEN_OP_STAR_EQUAL, 2 },
            { TOKEN_OP_SLASH_EQUAL, 2 },
            { TOKEN_OP_MOD_EQUAL, 2 },
            { TOKEN_OP_LEFT_EQUAL, 2 },
            { TOKEN_OP_RIGHT_EQUAL, 2 },
            { TOKEN_OP_BITWISE_AND_EQUAL, 2 },
            { TOKEN_OP_BITWISE_XOR_EQUAL, 2 },
            { TOKEN_OP_BITWISE_OR_EQUAL, 2 },
            { TOKEN_OP_COMMA, 1 }
        };
    }
}
