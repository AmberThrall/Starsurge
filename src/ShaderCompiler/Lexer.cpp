#include <iostream>
#include <algorithm>
#include "../../include/ShaderCompiler/Lexer.h"
#include "../../include/ShaderCompiler/Lexemes.h"
#include "../../include/Utils.h"

Starsurge::GLSL::Token::Token(std::string t_data, TokenType t_type, unsigned int t_lineno) : data(t_data), type(t_type), lineno(t_lineno) { }

bool Starsurge::GLSL::Lexer::IsDigit(char c) {
    return (c >= '0' && c <= '9');
}
bool Starsurge::GLSL::Lexer::IsLetter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
bool Starsurge::GLSL::Lexer::IsAlpha(char c) {
    return IsLetter(c) || IsDigit(c) || (c == '_');
}
bool Starsurge::GLSL::Lexer::IsHex(char c) {
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || IsDigit(c);
}
bool Starsurge::GLSL::Lexer::IsWhitespace(char c) {
    return c == ' ' || c == '\r' || c == '\t' || c == '\n' || c == '\f' || c == '\v';
}
bool Starsurge::GLSL::Lexer::IsType(std::string str) {
    return Starsurge::ElemOf(TYPES, TYPES_COUNT, str.c_str());
}
bool Starsurge::GLSL::Lexer::IsQualifier(std::string str) {
    if (Starsurge::ElemOf<const char*>(QUALIFIERS_STORAGE, QUALIFIERS_STORAGE_COUNT, str.c_str())) {
        return true;
    }
    else if (Starsurge::ElemOf<const char*>(QUALIFIERS_MISC, QUALIFIERS_MISC_COUNT, str.c_str())) {
        return true;
    }
    else if (Starsurge::ElemOf<const char*>(QUALIFIERS_INTERP, QUALIFIERS_INTERP_COUNT, str.c_str())) {
        return true;
    }
    else if (Starsurge::ElemOf<const char*>(QUALIFIERS_PREC, QUALIFIERS_PREC_COUNT, str.c_str())) {
        return true;
    }
    else if (Starsurge::ElemOf<const char*>(QUALIFIERS_MEMORY, QUALIFIERS_MEMORY_COUNT, str.c_str())) {
        return true;
    }
    else if (std::strcmp(QUALIFIERS_INVARIANT, str.c_str()) == 0) {
        return true;
    }
    else {
        return false;
    }
}
bool Starsurge::GLSL::Lexer::IsOctal(std::string str) {
    if (str == "") { return false; }
    if (str[0] != '0') { return false; }
    for (unsigned int i = 1; i < str.length(); ++i) {
        if (!(str[i] >= '0' || str[i] <= '7')) {
            return false;
        }
    }
    return true;
}

char Starsurge::GLSL::Lexer::Peek(int offset) {
    if (this->position+offset < this->code.length())
        return this->code[this->position+offset];
    return EOF;
}

char Starsurge::GLSL::Lexer::NextChar() {
    char r = this->code[this->position];
    this->position++;
    return r;
}

Starsurge::GLSL::Lexer::Lexer(std::string t_code) {
    this->code = t_code;
    this->position = 0;
    this->error = "";
    this->lineno = 1;
}

std::string Starsurge::GLSL::Lexer::GetError() {
    return this->error;
}

bool Starsurge::GLSL::Lexer::Read(Token & token) {
    char c = NextChar();

    while (IsWhitespace(c)) {
        if (c == '\n') { lineno++; }
        c = NextChar();
    }

    token.lineno = lineno;

    if (this->position > this->code.length()) {
        token.type = TOKEN_EOF;
        return true;
    }

    if (IsDigit(c) || (c == '.' && IsDigit(Peek()))) {
        std::string num = "";
        num += c;
        token.type = TOKEN_INT;

        if (c == '0' && (Peek() == 'x' || Peek() == 'X')) { // Is it a hex number?
            num += NextChar();

            while (IsHex(Peek())) {
                c = NextChar();
                num += c;
            }

            num = std::to_string(std::stoi(num, 0, 16)); // Convert to decimal
        }
        else {
            while (IsDigit(Peek()) || Peek() == '.') {
                c = NextChar();
                num += c;
            }

            size_t numDecimals = std::count(num.begin(), num.end(), '.');
            if (numDecimals > 1) {
                this->error = "More than one decimal in number.";
                return false;
            }

            if (num[0] == '0') {
                if (numDecimals == 1 && num[1] != '.') { // Error of the form 02.3
                    this->error = "Invalid decimal expression. Leading zero.";
                    return false;
                }
                else { //Bad octal: 058
                    for (unsigned int i = 1; i < num.size(); ++i) {
                        if (num[i] == '8' || num[i] == '9') {
                            this->error = "Invalid digit for base 8 number.";
                            return false;
                        }
                    }
                }
            }

            if (numDecimals == 1) { token.type = TOKEN_FLOAT; }

            if (Peek() == 'f' || Peek() == 'F') { // Floating point numbers.
                c = NextChar();
                num += c;
                token.type = TOKEN_FLOAT;
            }
        }

        if (token.type == TOKEN_INT && (Peek() == 'u' || Peek() == 'U')) {
            NextChar();
        }
        if (IsOctal(num)) {
            num = std::to_string(std::stoi(num, 0, 8));
        }
        token.data = num;
        return true;
    }
    else if (IsLetter(c) || c == '_') {
        std::string identifier = "";
        identifier += c;
        while (IsAlpha(Peek())) {
            c = NextChar();
            identifier += c;
        }

        token.data = identifier;
        if (identifier == "layout") { token.type = TOKEN_LAYOUT; }
        else if (identifier == "break") { token.type = TOKEN_BREAK; }
        else if (identifier == "continue") { token.type = TOKEN_CONTINUE; }
        else if (identifier == "do") { token.type = TOKEN_DO; }
        else if (identifier == "for") { token.type = TOKEN_FOR; }
        else if (identifier == "while") { token.type = TOKEN_WHILE; }
        else if (identifier == "switch") { token.type = TOKEN_SWITCH; }
        else if (identifier == "case") { token.type = TOKEN_CASE; }
        else if (identifier == "default") { token.type = TOKEN_DEFAULT; }
        else if (identifier == "if") { token.type = TOKEN_IF; }
        else if (identifier == "else") { token.type = TOKEN_ELSE; }
        else if (identifier == "true") { token.type = TOKEN_TRUE; }
        else if (identifier == "false") { token.type = TOKEN_FALSE; }
        else if (identifier == "return") { token.type = TOKEN_RETURN; }
        else if (identifier == "discard") { token.type = TOKEN_DISCARD; }
        else if (identifier == "precision") { token.type = TOKEN_PRECISION; }
        else if (identifier == "struct") { token.type = TOKEN_STRUCT; }
        else if (IsType(identifier)) { token.type = TOKEN_TYPE; }
        else if (IsQualifier(identifier)) { token.type = TOKEN_QUALIFIER; }
        else { token.type = TOKEN_IDENTIFIER; }
        return true;
    }
    else {
        char c1 = Peek();
        char c2 = Peek(1);
        switch (c) {
            case ';':
                token.type = TOKEN_SEMICOLON;
                token.data = ";";
                break;
            case '{':
                token.type = TOKEN_SCOPE_BEGIN;
                token.data = "{";
                break;
            case '}':
                token.type = TOKEN_SCOPE_END;
                token.data = "}";
                break;
            case '/': // Comments
                c = Peek();
                if (c == '/') {
                    while (c != '\n' && c != '\r' && c != EOF) { // It's a line comment, just keep going.
                        c = NextChar();
                    }
                    return Read(token);
                }
                else if (c == '*') { // It's a block comment.
                    c = NextChar();
                    while (true) {
                        c = NextChar();
                        if (c == EOF) {
                            this->error = "Block comment missing ending */";
                            return false;
                        }
                        if (c == '*' && Peek() == '/') {
                            NextChar();
                            return Read(token);
                        }
                    }
                }
                else if (c == '=') {
                    token.type = TOKEN_OP_SLASH_EQUAL;
                    token.data = "/"+NextChar();
                }
                else {
                    token.type = TOKEN_OP_SLASH;
                    token.data = "/";
                }
                break;
            case '.':
                token.type = TOKEN_OP_DOT;
                token.data = ".";
                break;
            case '+':
                token.type = TOKEN_OP_PLUS;
                c = Peek();
                if (c == '+') {
                    token.type = TOKEN_OP_INC;
                    token.data = "++";
                    NextChar();
                }
                else if (c == '=') {
                    token.type = TOKEN_OP_PLUS_EQUAL;
                    token.data = "+=";
                    NextChar();
                }
                else { token.data = "+"; }
                break;
            case '-':
                token.type = TOKEN_OP_MINUS;
                c = Peek();
                if (c == '-') {
                    token.type = TOKEN_OP_DEC;
                    token.data = "--";
                    NextChar();
                }
                else if (c == '=') {
                    token.type = TOKEN_OP_MINUS_EQUAL;
                    token.data = "-=";
                    NextChar();
                }
                else { token.data = "-"; }
                break;
            case '*':
                token.type = TOKEN_OP_STAR;
                c = Peek();
                if (c == '=') {
                    token.type = TOKEN_OP_STAR_EQUAL;
                    token.data = "*=";
                    NextChar();
                }
                else { token.data = "*"; }
                break;
            case '%':
                token.type = TOKEN_OP_MOD;
                c = Peek();
                if (c == '=') {
                    token.type = TOKEN_OP_MOD_EQUAL;
                    token.data = "%=";
                    NextChar();
                }
                else { token.data = "%"; }
                break;
            case '<':
                token.type = TOKEN_OP_LT;
                if (c1 == '<' && c2 == '=') {
                    token.type = TOKEN_OP_LEFT_EQUAL;
                    token.data = "<<=";
                    NextChar();
                    NextChar();
                }
                else if (c1 == '<') {
                    token.type = TOKEN_OP_LEFT;
                    token.data = "<<";
                    NextChar();
                }
                else if (c1 == '=') {
                    token.type = TOKEN_OP_LTEQ;
                    token.data = "<=";
                    NextChar();
                }
                else { token.data = "<"; }
                break;
            case '>':
                token.type = TOKEN_OP_GT;
                if (c1 == '>' && c2 == '=') {
                    token.type = TOKEN_OP_RIGHT_EQUAL;
                    token.data = ">>=";
                    NextChar();
                    NextChar();
                }
                else if (c1 == '>') {
                    token.type = TOKEN_OP_RIGHT;
                    token.data = ">>";
                    NextChar();
                }
                else if (c1 == '=') {
                    token.type = TOKEN_OP_GTEQ;
                    token.data = ">=";
                    NextChar();
                }
                else { token.data = ">"; }
                break;
            case '[':
                token.type = TOKEN_LEFT_BRACKET;
                token.data = "[";
                break;
            case ']':
                token.type = TOKEN_RIGHT_BRACKET;
                token.data = "]";
                break;
            case '(':
                token.type = TOKEN_LEFT_PAREN;
                token.data = "(";
                break;
            case ')':
                token.type = TOKEN_RIGHT_PAREN;
                token.data = ")";
                break;
            case '^':
                token.type = TOKEN_OP_BITWISE_XOR;
                c = Peek();
                if (c == '=') {
                    token.type = TOKEN_OP_BITWISE_XOR_EQUAL;
                    token.data = "^=";
                    NextChar();
                }
                else if (c == '^') {
                    token.type = TOKEN_OP_XOR;
                    token.data = "^^";
                    NextChar();
                }
                else { token.data = "^"; }
                break;
            case '|':
                token.type = TOKEN_OP_BITWISE_OR;
                c = Peek();
                if (c == '=') {
                    token.type = TOKEN_OP_BITWISE_OR_EQUAL;
                    token.data = "|=";
                    NextChar();
                }
                else if (c == '|') {
                    token.type = TOKEN_OP_OR;
                    token.data = "||";
                    NextChar();
                }
                else { token.data = "|"; }
                break;
            case '&':
                token.type = TOKEN_OP_BITWISE_AND;
                c = Peek();
                if (c == '=') {
                    token.type = TOKEN_OP_BITWISE_AND_EQUAL;
                    token.data = "&=";
                    NextChar();
                }
                else if (c == '&') {
                    token.type = TOKEN_OP_AND;
                    token.data = "&&";
                    NextChar();
                }
                else { token.data = "&"; }
                break;
            case '~':
                token.type = TOKEN_OP_TILDE;
                token.data = "~";
                break;
            case '=':
                token.type = TOKEN_OP_ASSIGN;
                c = Peek();
                if (c == '=') {
                    token.type = TOKEN_OP_EQUALS;
                    token.data = "==";
                    NextChar();
                }
                else { token.data = "="; }
                break;
            case '!':
                token.type = TOKEN_OP_BANG;
                c = Peek();
                if (c == '=') {
                    token.type = TOKEN_OP_NOT_EQUALS;
                    token.data = "!=";
                    NextChar();
                }
                else { token.data = "!"; }
                break;
            case ':':
                token.type = TOKEN_OP_COLON;
                token.data = ":";
                break;
            case ',':
                token.type = TOKEN_OP_COMMA;
                token.data = ",";
                break;
            case '?':
                token.type = TOKEN_OP_QUESTION;
                token.data = "?";
                break;
            default:
                this->error = std::string("Invalid character '")+c+std::string("' encountered.");
                return false;
        }
        return true;
    }

    return false;
}
