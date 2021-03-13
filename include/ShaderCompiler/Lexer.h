#pragma once
#include <string>
#include "Lexemes.h"

namespace Starsurge {
    namespace GLSL {
        class Parser;

        struct Token {
            Token(std::string t_data = "", TokenType t_type = TOKEN_EOF, unsigned int t_lineno = 0);
            std::string data;
            TokenType type;
            unsigned int lineno;
        };

        class Lexer {
        public:
            Lexer(std::string code = "");

            bool Read(Token & token);
            std::string GetError();
        protected:
            bool IsDigit(char c);
            bool IsLetter(char c);
            bool IsAlpha(char c);
            bool IsHex(char c);
            bool IsWhitespace(char c);
            bool IsType(std::string str);
            bool IsQualifier(std::string str);
            bool IsOctal(std::string str);
        private:
            friend Parser;
            char Peek(int offset = 0);
            char NextChar();

            std::string code;
            std::string error;
            unsigned int position;
            unsigned int lineno;
        };
    }
}
