#pragma once
#include <string>
#include <map>
#include "Lexer.h"
#include "AST.h"

namespace Starsurge {
    namespace GLSL {
        struct QualifierData {
            std::string storage;
            std::string misc;
            std::string interp;
            std::string precision;
            std::string memory;
            bool invariant;
        };

        struct ParseError {
            unsigned int lineno;
            std::string msg;
        };

        class Parser {
        public:
            Parser(std::string t_code = "");

            void SetCode(std::string t_code);
            AST GetAST();
            void Parse();
            std::vector<ParseError> GetErrors();
        private:
            void Error(std::string t_msg);
            ASTNodeExpression * Evaluate(ASTNodeExpression * expr, ASTNode * scope);
            std::string EvaluateType(ASTNodeExpression * expr, ASTNode * scope);
            ASTNodeProgram * Program();
            ASTNodeStatement * Statement(ASTNode * scope);
            ASTNode * Scope(ASTNode * parentScope, ASTNode * parent, ASTNode * target = NULL);
            ASTNodeExpression * Expression(ASTNode * scope, int precedence = 1);
            bool PeekVariableDeclaration(ASTNode * scope);
            ASTNode * VariableDeclaration(ASTNode * scope);
            ASTNodeFunction * FunctionDeclaration(ASTNode * scope);
            ASTNodeStruct * Struct(ASTNode * scope);
            ASTNodeCase * Case(ASTNode * scope, ASTNodeSwitch * parent);
            bool FieldDeclaration(ASTNode * scope, ASTNodeStruct * strct);
            QualifierData Qualifiers();
            ASTNodeExpression * UnaryPrimary(ASTNode * scope);
            ASTNodeExpression * UnaryPostfix(ASTNode * scope);
            ASTNodeExpression * Unary(ASTNode * scope);
            ASTNodeFunctionCall * FunctionCall(ASTNode * scope);
            int ArrayDeclare(ASTNode * scope);
            bool Operator();
            bool OperatorArithmetic();
            bool OperatorTest();
            bool OperatorAssign();
            bool OperatorBitwise();
            bool Type(ASTNode * scope);
            bool Variable(ASTNode * scope);
            bool Function(ASTNode * scope);
            ASTNodeStruct * FindStruct(std::string search, ASTNode * scope);
            ASTNodeVariableDeclaration * FindVariable(std::string search, ASTNode * scope);
            std::vector<ASTNodeFunction*> FindFunction(std::string search, ASTNode * scope);
            bool CompatibleTypes(std::string type1, std::string type2);

            unsigned int Tell();
            void Seek(unsigned int pos);
            bool AtEnd();
            Token Peek();
            Token Previous();
            Token Advance();
            bool Match(TokenType match);
            bool Match(std::vector<TokenType> matches);
            bool Check(TokenType match);
            bool Check(std::vector<TokenType> matches);

            AST ast;
            Lexer lexer;
            std::string code;
            std::vector<Token> tokens;
            unsigned int position;
            std::vector<ParseError> errors;
        };
    }
}
