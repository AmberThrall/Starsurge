#include "../../include/ShaderCompiler/AST.h"
#include "../../include/Utils.h"
#include "../../include/Logging.h"

Starsurge::GLSL::ASTNode::ASTNode(ASTNodeTypes t_nodeType) { nodeType = t_nodeType; parent = NULL; }
Starsurge::GLSL::ASTNodeProgram::ASTNodeProgram() : ASTNode(AST_NODE_PROGRAM) { }
Starsurge::GLSL::ASTNodeStatement::ASTNodeStatement() : ASTNode(AST_NODE_STATEMENT) { parent = NULL; }
Starsurge::GLSL::ASTNodeExpression::ASTNodeExpression() : ASTNode(AST_NODE_EXPRESSION) { parent = NULL; }
Starsurge::GLSL::ASTNodeUnary::ASTNodeUnary() : ASTNode(AST_NODE_UNARY) { parent = NULL; }
Starsurge::GLSL::ASTNodeIdentifier::ASTNodeIdentifier(std::string t_id) : id(t_id) {
    this->nodeType = AST_NODE_IDENTIFIER;
    parent = NULL;
}
Starsurge::GLSL::ASTNodeVariable::ASTNodeVariable(std::string t_name) : name(t_name) {
    parent = NULL;
    this->nodeType = AST_NODE_VARIABLE;
}
Starsurge::GLSL::ASTNodeFunction::ASTNodeFunction() {
    this->nodeType = AST_NODE_FUNCTION;
    parent = NULL;
}
Starsurge::GLSL::ASTNodeStruct::ASTNodeStruct() {
    this->nodeType = AST_NODE_STRUCT;
    parent = NULL;
}
Starsurge::GLSL::ASTNodeField::ASTNodeField() : ASTNode(AST_NODE_FIELD) { parent = NULL; arraySize = 0; }
Starsurge::GLSL::ASTNodeConstBool::ASTNodeConstBool(bool val) : value(val) { parent = NULL; this->nodeType = AST_NODE_CONST_BOOL; }
Starsurge::GLSL::ASTNodeConstInt::ASTNodeConstInt(int val) : value(val) { parent = NULL; this->nodeType = AST_NODE_CONST_INT; }
Starsurge::GLSL::ASTNodeConstFloat::ASTNodeConstFloat(float val) : value(val) { parent = NULL; this->nodeType = AST_NODE_CONST_FLOAT; }
Starsurge::GLSL::ASTNodeBinary::ASTNodeBinary(std::string t_op) : op(t_op) {
    parent = NULL;
    this->nodeType = AST_NODE_BINARY;
    this->children.push_back(NULL);
    this->children.push_back(NULL);
}
Starsurge::GLSL::ASTNodePrefixOp::ASTNodePrefixOp(std::string t_op) : op(t_op) { parent = NULL; this->nodeType = AST_NODE_PREFIX_OP; }
Starsurge::GLSL::ASTNodePostfixOp::ASTNodePostfixOp(std::string t_op) : op(t_op) { parent = NULL; this->nodeType = AST_NODE_POSTFIX_OP; }
Starsurge::GLSL::ASTNodeVariableDeclaration::ASTNodeVariableDeclaration() {
    parent = NULL;
    this->nodeType = AST_NODE_VARIABLE_DECLARATION;
    this->isInvariant = false;
    this->children.push_back(NULL);
    this->arraySize = 0;
}
Starsurge::GLSL::ASTNodeScope::ASTNodeScope() { parent = NULL; this->nodeType = AST_NODE_SCOPE; }
Starsurge::GLSL::ASTNodeExpressionStatement::ASTNodeExpressionStatement() { parent = NULL; this->nodeType = AST_NODE_EXPRESSION_STATEMENT; }
Starsurge::GLSL::ASTNodeEmptyStatement::ASTNodeEmptyStatement() { parent = NULL; this->nodeType = AST_NODE_EMPTY_STATEMENT; }
Starsurge::GLSL::ASTNodeFor::ASTNodeFor() {
    parent = NULL;
    this->nodeType = AST_NODE_FOR;
    this->children.push_back(NULL);
    this->children.push_back(NULL);
    this->children.push_back(NULL);
    this->children.push_back(NULL);
}
Starsurge::GLSL::ASTNodeWhile::ASTNodeWhile() {
    parent = NULL;
    this->nodeType = AST_NODE_WHILE;
    this->children.push_back(NULL);
    this->children.push_back(NULL);
}
Starsurge::GLSL::ASTNodeFunctionCall::ASTNodeFunctionCall(std::string t_function) : function(t_function) {
    parent = NULL;
    this->nodeType = AST_NODE_FUNCTION_CALL;
}
Starsurge::GLSL::ASTNodeDo::ASTNodeDo() {
    parent = NULL;
    this->nodeType = AST_NODE_DO;
    this->children.push_back(NULL);
    this->children.push_back(NULL);
}
Starsurge::GLSL::ASTNodeSwitch::ASTNodeSwitch() {
    parent = NULL;
    this->nodeType = AST_NODE_SWITCH;
    this->children.push_back(NULL);
}
Starsurge::GLSL::ASTNodeCase::ASTNodeCase() {
    parent = NULL;
    this->nodeType = AST_NODE_CASE;
    this->isDefault = false;
    this->children.push_back(NULL);
    this->children.push_back(NULL);
}
Starsurge::GLSL::ASTNodeIf::ASTNodeIf() {
    parent = NULL;
    this->nodeType = AST_NODE_IF;
    this->children.push_back(NULL);
    this->children.push_back(NULL);
    this->children.push_back(NULL);
}
Starsurge::GLSL::ASTNodeTernary::ASTNodeTernary() {
    parent = NULL;
    this->nodeType = AST_NODE_TERNARY;
    this->children.push_back(NULL);
    this->children.push_back(NULL);
    this->children.push_back(NULL);
}
Starsurge::GLSL::ASTNodeParenExpression::ASTNodeParenExpression() { parent = NULL; this->nodeType = AST_NODE_PAREN_EXPRESSION; }
Starsurge::GLSL::ASTNodeJump::ASTNodeJump(std::string t_keyword) : keyword(t_keyword) { parent = NULL; this->nodeType = AST_NODE_JUMP; }

Starsurge::GLSL::ASTNode::~ASTNode() {
    /*for (unsigned int i = 0; i < this->children.size(); ++i) {
        if (children[i] != NULL) {
            delete children[i];
        }
    }
    children.clear();*/
}

Starsurge::GLSL::ASTNode* Starsurge::GLSL::ASTNode::GetChild(unsigned int i) {
    if (i >= children.size()) {
        return NULL;
    }
    return children[i];
}

void Starsurge::GLSL::ASTNode::RemoveChild(ASTNode * node) {
    for (unsigned int i = 0; i < children.size(); ++i) {
        if (children[i] == node) {
            children.erase(children.begin()+i);
        }
    }
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::ASTNode::GetParent() {
    return parent;
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::ASTNode::GetScope() {
    if (nodeType == AST_NODE_PROGRAM || nodeType == AST_NODE_FUNCTION || nodeType == AST_NODE_SCOPE) {
        return this;
    }
    return parent->GetScope();
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::ASTNode::GetHigherScope() {
    ASTNode * currentScope = GetScope();
    if (currentScope->GetParent() == NULL) {
        return NULL;
    }
    return currentScope->GetParent()->GetScope();
}

unsigned int Starsurge::GLSL::ASTNode::ChildCount() {
    return children.size();
}

bool Starsurge::GLSL::ASTNode::IsChild(ASTNode * node) {
    for (unsigned int i = 0; i < children.size(); ++i) {
        if (children[i] == node) {
            return true;
        }
    }

    return false;
}

const Starsurge::GLSL::ASTNodeTypes Starsurge::GLSL::ASTNode::GetNodeType() {
    return this->nodeType;
}

std::vector<Starsurge::GLSL::ASTNode*> Starsurge::GLSL::ASTNode::GetAllChildren() {
    return this->children;
}

void Starsurge::GLSL::ASTNode::AddChild(ASTNode * node) {
    if (!IsChild(node)) {
        children.push_back(node);
        node->parent = this;
    }
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::ASTNodeBinary::GetLeft() {
    return this->children[0];
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::ASTNodeBinary::GetRight() {
    return this->children[1];
}

void Starsurge::GLSL::ASTNodeBinary::SetLeft(ASTNode * node) {
    this->children[0] = node;
    node->parent = this;
}

void Starsurge::GLSL::ASTNodeBinary::SetRight(ASTNode * node) {
    this->children[1] = node;
    node->parent = this;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeVariableDeclaration::GetInitialValue() {
    return (ASTNodeExpression*)this->children[0];
}

void Starsurge::GLSL::ASTNodeVariableDeclaration::SetInitialValue(ASTNodeExpression * expr) {
    this->children[0] = expr;
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::ASTNodeFor::GetDeclares() {
    return children[0];
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::ASTNodeFor::GetCondition() {
    return children[1];
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::ASTNodeFor::GetExpression() {
    return children[2];
}

Starsurge::GLSL::ASTNodeScope * Starsurge::GLSL::ASTNodeFor::GetScope() {
    return (ASTNodeScope*)children[3];
}

void Starsurge::GLSL::ASTNodeFor::SetDeclares(ASTNode * node) {
    this->children[0] = node;
    node->parent = this;
}

void Starsurge::GLSL::ASTNodeFor::SetCondition(ASTNode * node) {
    this->children[1] = node;
    node->parent = this;
}

void Starsurge::GLSL::ASTNodeFor::SetExpression(ASTNode * node) {
    this->children[2] = node;
    node->parent = this;
}

void Starsurge::GLSL::ASTNodeFor::SetScope(ASTNodeScope * scope) {
    this->children[3] = scope;
    scope->parent = this;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeWhile::GetCondition() {
    return (ASTNodeExpression*)this->children[0];
}
Starsurge::GLSL::ASTNodeScope * Starsurge::GLSL::ASTNodeWhile::GetScope() {
    return (ASTNodeScope*)this->children[1];
}
void Starsurge::GLSL::ASTNodeWhile::SetCondition(ASTNodeExpression * node) {
    this->children[0] = node;
    node->parent = this;
}
void Starsurge::GLSL::ASTNodeWhile::SetScope(ASTNodeScope * scope) {
    this->children[1] = scope;
    scope->parent = this;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeDo::GetCondition() {
    return (ASTNodeExpression*)this->children[1];
}
Starsurge::GLSL::ASTNodeScope * Starsurge::GLSL::ASTNodeDo::GetScope() {
    return (ASTNodeScope*)this->children[0];
}
void Starsurge::GLSL::ASTNodeDo::SetCondition(ASTNodeExpression * node) {
    this->children[1] = node;
    node->parent = this;
}
void Starsurge::GLSL::ASTNodeDo::SetScope(ASTNodeScope * scope) {
    this->children[0] = scope;
    scope->parent = this;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeSwitch::GetInput() {
    return (ASTNodeExpression*)this->children[0];
}
std::vector<Starsurge::GLSL::ASTNodeCase*> Starsurge::GLSL::ASTNodeSwitch::GetCases() {
    return GetChildren<ASTNodeCase>(AST_NODE_CASE);
}
void Starsurge::GLSL::ASTNodeSwitch::SetInput(ASTNodeExpression * node) {
    this->children[0] = node;
    node->parent = this;
}
void Starsurge::GLSL::ASTNodeSwitch::AddCase(ASTNodeCase * node) {
    AddChild(node);
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeCase::GetCondition() {
    return (ASTNodeExpression*)this->children[0];
}
Starsurge::GLSL::ASTNodeScope * Starsurge::GLSL::ASTNodeCase::GetScope() {
    return (ASTNodeScope*)this->children[1];
}
void Starsurge::GLSL::ASTNodeCase::SetCondition(ASTNodeExpression * node) {
    this->children[0] = node;
    node->parent = this;
}
void Starsurge::GLSL::ASTNodeCase::SetScope(ASTNodeScope * scope) {
    this->children[1] = scope;
    scope->parent = this;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeIf::GetCondition() {
    return (ASTNodeExpression*)this->children[0];
}
Starsurge::GLSL::ASTNodeScope * Starsurge::GLSL::ASTNodeIf::GetScope() {
    return (ASTNodeScope*)this->children[1];
}
Starsurge::GLSL::ASTNodeIf * Starsurge::GLSL::ASTNodeIf::GetNext() {
    return (ASTNodeIf*)this->children[2];
}
void Starsurge::GLSL::ASTNodeIf::SetCondition(ASTNodeExpression * node) {
    this->children[0] = node;
    node->parent = this;
}
void Starsurge::GLSL::ASTNodeIf::SetScope(ASTNodeScope * scope) {
    this->children[1] = scope;
    scope->parent = this;
}
void Starsurge::GLSL::ASTNodeIf::SetNext(ASTNodeIf * ifNode) {
    this->children[2] = ifNode;
    ifNode->parent = this;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeTernary::GetCondition() {
    return (ASTNodeExpression*)this->children[0];
}
Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeTernary::GetTrue() {
    return (ASTNodeExpression*)this->children[1];
}
Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::ASTNodeTernary::GetFalse() {
    return (ASTNodeExpression*)this->children[2];
}
void Starsurge::GLSL::ASTNodeTernary::SetCondition(ASTNodeExpression * node) {
    this->children[0] = node;
    node->parent = this;
}
void Starsurge::GLSL::ASTNodeTernary::SetTrue(ASTNodeExpression * node) {
    this->children[1] = node;
    node->parent = this;
}
void Starsurge::GLSL::ASTNodeTernary::SetFalse(ASTNodeExpression * node) {
    this->children[2] = node;
    node->parent = this;
}

Starsurge::GLSL::AST::AST(ASTNode * t_root) : root(t_root) {}

Starsurge::GLSL::AST::~AST() {
    if (root != NULL) {
        //delete root;
    }
}

std::string Starsurge::GLSL::AST::ToDOT(ASTNode * start) {
    if (start == NULL) {
        start = root;
    }
    std::string dot = "graph ast {\n";

    DOTWriteVertex(start, dot);

    dot += "}";
    return dot;
}

std::string Starsurge::GLSL::ASTNode::ToString() {
    return "ASTNode";
}

std::string Starsurge::GLSL::ASTNodeProgram::ToString() {
    return "Program";
}

std::string Starsurge::GLSL::ASTNodeStatement::ToString() {
    return "Statement";
}

std::string Starsurge::GLSL::ASTNodeExpression::ToString() {
    return "Expression";
}

std::string Starsurge::GLSL::ASTNodeFunctionCall::ToString() {
    return function + "()";
}

std::string Starsurge::GLSL::ASTNodeUnary::ToString() {
    return "Unary";
}

std::string Starsurge::GLSL::ASTNodeIdentifier::ToString() {
    return id;
}

std::string Starsurge::GLSL::ASTNodeVariable::ToString() {
    return name;
}

std::string Starsurge::GLSL::ASTNodeFor::ToString() {
    return "For";
}

std::string Starsurge::GLSL::ASTNodeDo::ToString() {
    return "Do";
}

std::string Starsurge::GLSL::ASTNodeParenExpression::ToString() {
    return "()";
}

std::string Starsurge::GLSL::ASTNodeVariableDeclaration::ToString() {
    std::string str = "";
    if (storage != "") str += storage + " ";
    if (misc != "") str += misc + " ";
    if (interp != "") str += interp + " ";
    if (precision != "") str += precision + " ";
    if (memory != "") str += memory + " ";
    if (isInvariant) str += "invariant ";
    str += type+" ";
    str += name;
    if (arraySize > 0) str += "[" + std::to_string(arraySize) + "]";
    return str;
}

std::string Starsurge::GLSL::ASTNodeFunction::ToString() {
    std::string str = "";
    if (storage != "") str += storage + " ";
    if (misc != "") str += misc + " ";
    if (interp != "") str += interp + " ";
    if (precision != "") str += precision + " ";
    if (memory != "") str += memory + " ";
    if (isInvariant) str += "invariant ";
    str += returnType+" "+name+"(";
    for (unsigned int i = 0; i < parameters.size(); ++i) {
        if (i > 0) { str += ", "; }
        if (parameters[i].qualifier != "") { str += parameters[i].qualifier + " "; }
        str += parameters[i].type + " " + parameters[i].name;
    }
    str += ")";
    return str;
}

std::string Starsurge::GLSL::ASTNodeScope::ToString() {
    return "{}";
}

std::string Starsurge::GLSL::ASTNodeStruct::ToString() {
    return "struct "+name;
}

std::string Starsurge::GLSL::ASTNodeField::ToString() {
    std::string str =  type+" "+name;
    if (arraySize > 0) str += "[" + std::to_string(arraySize) + "]";
    return str;
}

std::string Starsurge::GLSL::ASTNodeConstBool::ToString() {
    return std::string(value ? "true" : "false");
}

std::string Starsurge::GLSL::ASTNodeConstInt::ToString() {
    return std::to_string(value);
}

std::string Starsurge::GLSL::ASTNodeConstFloat::ToString() {
    return std::to_string(value);
}

std::string Starsurge::GLSL::ASTNodeBinary::ToString() {
    return "Binary ("+op+")";
}

std::string Starsurge::GLSL::ASTNodePrefixOp::ToString() {
    return "Prefix ("+op+")";
}

std::string Starsurge::GLSL::ASTNodePostfixOp::ToString() {
    return "Postfix ("+op+")";
}

std::string Starsurge::GLSL::ASTNodeExpressionStatement::ToString() {
    return "Expression Statement";
}

std::string Starsurge::GLSL::ASTNodeEmptyStatement::ToString() {
    return "Empty Statement";
}

std::string Starsurge::GLSL::ASTNodeWhile::ToString() {
    return "While";
}

std::string Starsurge::GLSL::ASTNodeSwitch::ToString() {
    return "Switch";
}

std::string Starsurge::GLSL::ASTNodeIf::ToString() {
    std::string ret = "";
    if (GetParent()->GetNodeType() == AST_NODE_IF) {
        ret = "Else";
    }
    if (GetCondition() != NULL) {
        if (ret != "") ret += " ";
        ret += "If";
    }
    return ret;
}

std::string Starsurge::GLSL::ASTNodeCase::ToString() {
    if (isDefault) return "Default";
    return "Case";
}

std::string Starsurge::GLSL::ASTNodeTernary::ToString() {
    return "?";
}

std::string Starsurge::GLSL::ASTNodeJump::ToString() {
    return keyword;
}

void Starsurge::GLSL::AST::DOTWriteVertex(ASTNode * node, std::string & dot) {
    if (node == NULL) {
        return;
    }

    dot += "    node"+std::to_string((int)node)+" [label=\""+node->ToString()+"\"];\n";
    std::vector<ASTNode*> children = node->GetAllChildren();
    for (unsigned int i = 0; i < children.size(); ++i) {
        DOTWriteVertex(children[i], dot);
        if (children[i] != NULL) {
            dot += "    node"+std::to_string((int)node)+" -- node"+std::to_string((int)children[i])+";\n";
        }
    }
}
