#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Lexemes.h"

namespace Starsurge {
    namespace GLSL {
        enum ASTNodeTypes {
            AST_NODE,
            AST_NODE_PROGRAM,
            AST_NODE_STATEMENT,
            AST_NODE_EXPRESSION,
            AST_NODE_IDENTIFIER,
            AST_NODE_VARIABLE,
            AST_NODE_FUNCTION,
            AST_NODE_STRUCT,
            AST_NODE_FIELD,
            AST_NODE_CONST_BOOL,
            AST_NODE_CONST_INT,
            AST_NODE_CONST_DOUBLE,
            AST_NODE_CONST_FLOAT,
            AST_NODE_BINARY,
            AST_NODE_UNARY,
            AST_NODE_PREFIX_OP,
            AST_NODE_POSTFIX_OP,
            AST_NODE_VARIABLE_DECLARATION,
            AST_NODE_GLOBAL_VARIABLE_DECLARATION,
            AST_NODE_SCOPE,
            AST_NODE_EXPRESSION_STATEMENT,
            AST_NODE_EMPTY_STATEMENT,
            AST_NODE_FOR,
            AST_NODE_WHILE,
            AST_NODE_FUNCTION_CALL,
            AST_NODE_DO,
            AST_NODE_SWITCH,
            AST_NODE_CASE,
            AST_NODE_TERNARY,
            AST_NODE_PAREN_EXPRESSION,
            AST_NODE_IF,
            AST_NODE_JUMP
        };

        class ASTNode {
        public:
            ASTNode(ASTNodeTypes t_nodeType = AST_NODE);
            ~ASTNode();
            const ASTNodeTypes GetNodeType();
            ASTNode* GetChild(unsigned int i);
            std::vector<ASTNode*> GetAllChildren();
            void AddChild(ASTNode * node);
            void RemoveChild(ASTNode * node);
            ASTNode * GetParent();
            ASTNode * GetScope();
            ASTNode * GetHigherScope();
            unsigned int ChildCount();
            bool IsChild(ASTNode * node);

            virtual std::string ToString();

            template<typename T>
            std::vector<T*> GetChildren(ASTNodeTypes nodeType) {
                std::vector<T*> res;
                for (unsigned int i = 0; i < children.size(); ++i) {
                    if (children[i]->GetNodeType() == nodeType) {
                        res.push_back((T*)children[i]);
                    }
                }
                return res;
            }
        protected:
            ASTNodeTypes nodeType;
            std::vector<ASTNode*> children;
            ASTNode * parent;

            // Friend class all children so they can access parent.
            friend class ASTNodeProgram;
            friend class ASTNodeStatement;
            friend class ASTNodeExpression;
            friend class ASTNodeUnary;
            friend class ASTNodeBinary;
            friend class ASTNodePrefixOp;
            friend class ASTNodePostfixOp;
            friend class ASTNodeIdentifier;
            friend class ASTNodeVariable;
            friend class ASTNodeFunction;
            friend class ASTNodeStruct;
            friend class ASTNodeField;
            friend class ASTNodeConstBool;
            friend class ASTNodeConstNumber;
            friend class ASTNodeVariableDeclaration;
            friend class ASTNodeGlobalVariableDeclaration;
            friend class ASTNodeScope;
            friend class ASTNodeExpressionStatement;
            friend class ASTNodeEmptyStatement;
            friend class ASTNodeFor;
            friend class ASTNodeWhile;
            friend class ASTNodeFunctionCall;
            friend class ASTNodeDo;
            friend class ASTNodeSwitch;
            friend class ASTNodeCase;
            friend class ASTNodeTernary;
            friend class ASTNodeParenExpression;
            friend class ASTNodeIf;
            friend class ASTNodeJump;
        };

        class ASTNodeProgram : public ASTNode {
        public:
            ASTNodeProgram();
            virtual std::string ToString();
        };

        class ASTNodeStatement : public ASTNode {
        public:
            ASTNodeStatement();
            virtual std::string ToString();
        };

        class ASTNodeFor : public ASTNodeStatement {
        public:
            ASTNodeFor();
            virtual std::string ToString();

            ASTNode * GetDeclares();
            ASTNode * GetCondition();
            ASTNode * GetExpression();
            ASTNodeScope * GetScope();
            void SetDeclares(ASTNode * node);
            void SetCondition(ASTNode * node);
            void SetExpression(ASTNode * node);
            void SetScope(ASTNodeScope * scope);
        };

        class ASTNodeWhile : public ASTNodeStatement {
        public:
            ASTNodeWhile();
            virtual std::string ToString();

            ASTNodeExpression * GetCondition();
            ASTNodeScope * GetScope();
            void SetCondition(ASTNodeExpression * node);
            void SetScope(ASTNodeScope * scope);
        };

        class ASTNodeDo : public ASTNodeStatement {
        public:
            ASTNodeDo();
            virtual std::string ToString();

            ASTNodeExpression * GetCondition();
            ASTNodeScope * GetScope();
            void SetCondition(ASTNodeExpression * node);
            void SetScope(ASTNodeScope * scope);
        };

        class ASTNodeCase : public ASTNodeStatement {
        public:
            ASTNodeCase();
            virtual std::string ToString();
            bool isDefault;

            ASTNodeExpression * GetCondition();
            ASTNodeScope * GetScope();
            void SetCondition(ASTNodeExpression * node);
            void SetScope(ASTNodeScope * scope);
        };

        class ASTNodeSwitch : public ASTNodeStatement {
        public:
            ASTNodeSwitch();
            virtual std::string ToString();

            ASTNodeExpression * GetInput();
            std::vector<ASTNodeCase*> GetCases();
            void SetInput(ASTNodeExpression * node);
            void AddCase(ASTNodeCase * node);
        };

        class ASTNodeIf : public ASTNodeStatement {
        public:
            ASTNodeIf();
            virtual std::string ToString();

            ASTNodeExpression * GetCondition();
            ASTNodeScope * GetScope();
            ASTNodeIf * GetNext();
            void SetCondition(ASTNodeExpression * node);
            void SetScope(ASTNodeScope * scope);
            void SetNext(ASTNodeIf * ifNode);
        };

        class ASTNodeJump : public ASTNodeStatement {
        public:
            ASTNodeJump(std::string t_keyword);
            virtual std::string ToString();
            std::string keyword;
        };

        class ASTNodeExpressionStatement : public ASTNodeStatement {
        public:
            ASTNodeExpressionStatement();
            virtual std::string ToString();
        };

        class ASTNodeExpression : public ASTNode {
        public:
            ASTNodeExpression();
            virtual std::string ToString();
        };

        class ASTNodeFunctionCall : public ASTNodeExpression {
        public:
            ASTNodeFunctionCall(std::string t_function);
            virtual std::string ToString();
            std::string function;
        };

        class ASTNodeUnary : public ASTNode {
        public:
            ASTNodeUnary();
            virtual std::string ToString();
        };

        class ASTNodeBinary: public ASTNodeExpression {
        public:
            ASTNodeBinary(std::string t_op);
            std::string op;
            ASTNode * GetLeft();
            ASTNode * GetRight();
            void SetLeft(ASTNode * node);
            void SetRight(ASTNode * node);
            virtual std::string ToString();

            void AddChild(ASTNode * node) {} // Disable the default add child.
        };

        class ASTNodePrefixOp: public ASTNodeExpression {
        public:
            ASTNodePrefixOp(std::string t_op);
            virtual std::string ToString();
            std::string op;
        };

        class ASTNodePostfixOp: public ASTNodeExpression {
        public:
            ASTNodePostfixOp(std::string t_op);
            virtual std::string ToString();
            std::string op;
        };

        class ASTNodeIdentifier : public ASTNodeExpression {
        public:
            ASTNodeIdentifier(std::string t_id);
            virtual std::string ToString();
            std::string id;
        };

        class ASTNodeVariableDeclaration : public ASTNodeStatement {
        public:
            ASTNodeVariableDeclaration();
            virtual std::string ToString();
            std::string storage;
            std::string misc;
            std::string interp;
            std::string precision;
            std::string memory;
            bool isInvariant;
            std::string name;
            std::string type;
            int arraySize;

            ASTNodeExpression * GetInitialValue();
            void SetInitialValue(ASTNodeExpression * expr);
        };

        class ASTNodeVariable : public ASTNodeExpression {
        public:
            ASTNodeVariable(std::string t_name);
            virtual std::string ToString();
            std::string name;
        };

        struct FunctionParameter {
            std::string qualifier;
            std::string type;
            std::string name;
            int arraySize;
        };

        class ASTNodeFunction : public ASTNodeStatement {
        public:
            ASTNodeFunction();
            virtual std::string ToString();
            std::string name;
            std::string returnType;
            std::string storage;
            std::string misc;
            std::string interp;
            std::string precision;
            std::string memory;
            bool isInvariant;
            std::vector<FunctionParameter> parameters;
        };

        class ASTNodeScope : public ASTNodeStatement {
        public:
            ASTNodeScope();
            virtual std::string ToString();
        };

        class ASTNodeStruct : public ASTNodeStatement {
        public:
            ASTNodeStruct();
            virtual std::string ToString();
            std::string name;
        };

        class ASTNodeField : public ASTNode {
        public:
            ASTNodeField();
            virtual std::string ToString();
            std::string name;
            std::string type;
            int arraySize;
        };

        class ASTNodeParenExpression : public ASTNodeExpression {
        public:
            ASTNodeParenExpression();
            virtual std::string ToString();
        };

        class ASTNodeConstBool : public ASTNodeExpression {
        public:
            ASTNodeConstBool(bool val);
            virtual std::string ToString();
            bool value;
        };

        class ASTNodeConstInt : public ASTNodeExpression {
        public:
            ASTNodeConstInt(int val);
            virtual std::string ToString();
            int value;
        };

        class ASTNodeConstFloat : public ASTNodeExpression {
        public:
            ASTNodeConstFloat(float val);
            virtual std::string ToString();
            float value;
        };

        class ASTNodeEmptyStatement : public ASTNodeStatement {
        public:
            ASTNodeEmptyStatement();
            virtual std::string ToString();
        };

        class ASTNodeTernary : public ASTNodeExpression {
        public:
            ASTNodeTernary();
            virtual std::string ToString();

            ASTNodeExpression * GetCondition();
            ASTNodeExpression * GetTrue();
            ASTNodeExpression * GetFalse();
            void SetCondition(ASTNodeExpression * node);
            void SetTrue(ASTNodeExpression * node);
            void SetFalse(ASTNodeExpression * node);
        };

        class AST {
        public:
            AST(ASTNode * t_root = NULL);
            ~AST();
            ASTNode * root;

            std::string ToDOT(ASTNode * start = NULL);
        private:
            void DOTWriteVertex(ASTNode * node, std::string & dot);
        };
    }
}
