#include "../../include/ShaderCompiler/Parser.h"
#include "../../include/Utils.h"
#include "../../include/Logging.h"
#include "../../include/ShaderCompiler/Lexemes.h"

Starsurge::GLSL::Parser::Parser(std::string t_code) {
    SetCode(t_code);
}

void Starsurge::GLSL::Parser::SetCode(std::string t_code) {
    code = t_code;
    lexer = Lexer(code);
}

Starsurge::GLSL::AST Starsurge::GLSL::Parser::GetAST() {
    return ast;
}


unsigned int Starsurge::GLSL::Parser::Tell() {
    return position;
}

void Starsurge::GLSL::Parser::Seek(unsigned int pos) {
    if (pos <= tokens.size()) {
        position = pos;
    }
}

bool Starsurge::GLSL::Parser::AtEnd() {
    return Peek().type == TOKEN_EOF;
}

Starsurge::GLSL::Token Starsurge::GLSL::Parser::Peek() {
    return tokens[position];
}

Starsurge::GLSL::Token Starsurge::GLSL::Parser::Previous() {
    if (position == 0) {
        return tokens[0]; // Safety net.
    }
    return tokens[position-1];
}

Starsurge::GLSL::Token Starsurge::GLSL::Parser::Advance() {
    if (!AtEnd()) {
        position++;
    }
    return Previous();
}

bool Starsurge::GLSL::Parser::Match(TokenType match) {
    std::vector<TokenType> matches = { match };
    return Match(matches);
}

bool Starsurge::GLSL::Parser::Match(std::vector<TokenType> matches) {
    for (unsigned int i = 0; i < matches.size(); ++i) {
        if (Check(matches[i])) {
            Advance();
            return true;
        }
    }

    return false;
}

bool Starsurge::GLSL::Parser::Check(TokenType match) {
    if (AtEnd()) {
        return false;
    }
    return (Peek().type == match);
}

bool Starsurge::GLSL::Parser::Check(std::vector<TokenType> matches) {
    for (unsigned int i = 0; i < matches.size(); ++i) {
        if (Check(matches[i])) {
            return true;
        }
    }

    return false;
}

std::vector<Starsurge::GLSL::ParseError> Starsurge::GLSL::Parser::GetErrors() {
    return errors;
}

void Starsurge::GLSL::Parser::Error(std::string t_msg) {
    ParseError newError;
    newError.lineno = Peek().lineno;
    newError.msg = t_msg;
    errors.push_back(newError);
}

void Starsurge::GLSL::Parser::Parse() {
    tokens.clear();
    position = 0;

    // Use the lexer to convert the source code into
    while (true) {
        Token nextToken;
        bool success = lexer.Read(nextToken);
        if (!success) {
            Error(lexer.error);
            return;
        }
        tokens.push_back(nextToken);
        if (nextToken.type == TOKEN_EOF) { break; }
    }

    ast.root = Program();
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::Parser::Evaluate(ASTNodeExpression * expr, ASTNode * scope) {
    if (!expr) { return NULL; }
    else if (expr->GetNodeType() == AST_NODE_CONST_INT) { return new ASTNodeConstInt(((ASTNodeConstInt*)expr)->value); }
    else if (expr->GetNodeType() == AST_NODE_CONST_FLOAT) { return new ASTNodeConstFloat(((ASTNodeConstFloat*)expr)->value); }
    else if (expr->GetNodeType() == AST_NODE_CONST_BOOL) { return new ASTNodeConstInt(((ASTNodeConstBool*)expr)->value ? 1 : 0); }
    else if (expr->GetNodeType() == AST_NODE_VARIABLE) {
        ASTNodeVariable * cast = (ASTNodeVariable*)expr;
        ASTNodeVariableDeclaration * var = FindVariable(cast->name, scope);
        if (var && var->storage == "const") {
            return Evaluate(var->GetInitialValue(), scope);
        }
        else {
            return NULL;
        }
    }
    else if (expr->GetNodeType() == AST_NODE_PREFIX_OP) {
        ASTNodePrefixOp * prefix = (ASTNodePrefixOp*)expr;
        if (prefix->ChildCount() != 1) { return NULL; }
        ASTNodeExpression * rhs = Evaluate((ASTNodeExpression*)prefix->GetChild(0), scope);
        if (!rhs) { return NULL; }

        if (prefix->op == "+") {
            if (rhs->GetNodeType() == AST_NODE_CONST_FLOAT) { return new ASTNodeConstFloat(std::abs(((ASTNodeConstFloat*)rhs)->value)); }
            return new ASTNodeConstInt(std::abs(((ASTNodeConstInt*)rhs)->value));
        }
        else if (prefix->op == "-") {
            if (rhs->GetNodeType() == AST_NODE_CONST_FLOAT) { return new ASTNodeConstFloat(-(((ASTNodeConstFloat*)rhs)->value)); }
            return new ASTNodeConstInt(-(((ASTNodeConstInt*)rhs)->value));
        }
        else if (prefix->op == "~") {
            if (rhs->GetNodeType() == AST_NODE_CONST_FLOAT) { return NULL; }
            return new ASTNodeConstInt(~(((ASTNodeConstInt*)rhs)->value));
        }
        else if (prefix->op == "!") {
            if (rhs->GetNodeType() == AST_NODE_CONST_FLOAT) { return NULL; }
            return new ASTNodeConstInt(!(((ASTNodeConstInt*)rhs)->value));
        }
        else { return NULL; }
    }
    else if (expr->GetNodeType() == AST_NODE_PAREN_EXPRESSION) {
        return Evaluate((ASTNodeExpression*)expr->GetChild(0), scope);
    }
    else if (expr->GetNodeType() == AST_NODE_POSTFIX_OP) {
        ASTNodePostfixOp * postfix = (ASTNodePostfixOp*)expr;
        if (postfix->op == "[]" && postfix->GetChild(0)->GetNodeType() == AST_NODE_VARIABLE) {
            ASTNodeVariableDeclaration * var = FindVariable(((ASTNodeVariable*)postfix->GetChild(0))->name, scope);

            if (var && var->storage == "const") {
                ASTNodeExpression * index = Evaluate((ASTNodeExpression*)postfix->GetChild(1), scope);
                if (!index || index->GetNodeType() != AST_NODE_CONST_INT) { return NULL; }

                int index_int = ((ASTNodeConstInt*)index)->value;
                if (index_int < 0 || index_int >= var->arraySize || var->GetInitialValue()->ChildCount() <= (unsigned int)index_int) { return NULL; }

                return Evaluate((ASTNodeExpression*)var->GetInitialValue()->GetChild(index_int), scope);
            }
            else { return NULL; }
        }
        else { return NULL; }
    }
    else if (expr->GetNodeType() == AST_NODE_BINARY) {
        ASTNodeBinary * binary = (ASTNodeBinary*)expr;
        if (binary->op == ".") {
            ASTNodeExpression * lhs = (ASTNodeExpression*)binary->GetLeft();
            ASTNodeExpression * rhs = (ASTNodeExpression*)binary->GetRight();
            if (rhs->GetNodeType() == AST_NODE_FUNCTION_CALL) {
                ASTNodeFunctionCall * cast = (ASTNodeFunctionCall*)rhs;
                if (cast->function == "length") {
                    std::string lhsType = EvaluateType(lhs, scope);
                    size_t leftBracket = lhsType.find("[");
                    size_t rightBracket = lhsType.find("]");
                    if (leftBracket != std::string::npos && rightBracket != std::string::npos) {
                        std::string substr = lhsType.substr(leftBracket, rightBracket-leftBracket-1);
                        if (substr == "") { return NULL; }
                        int size = std::stoi(substr);
                        return new ASTNodeConstInt(size);
                    }
                    else { return NULL; }
                }
                else { return NULL; }
            }
            else { return NULL; }
        }

        ASTNodeExpression * lhs = Evaluate((ASTNodeExpression*)binary->GetLeft(), scope);
        ASTNodeExpression * rhs = Evaluate((ASTNodeExpression*)binary->GetRight(), scope);
        if (!lhs || !rhs) { return NULL; }

        float lhs_v = 0, rhs_v = 0, ret_v = 0;
        if (lhs->GetNodeType() == AST_NODE_CONST_INT) { lhs_v = (float)((ASTNodeConstInt*)lhs)->value; }
        else if (lhs->GetNodeType() == AST_NODE_CONST_FLOAT) { lhs_v = ((ASTNodeConstFloat*)lhs)->value; }
        else { return NULL; }

        if (rhs->GetNodeType() == AST_NODE_CONST_INT) { rhs_v = (float)((ASTNodeConstInt*)rhs)->value; }
        else if (rhs->GetNodeType() == AST_NODE_CONST_FLOAT) { rhs_v = ((ASTNodeConstFloat*)rhs)->value; }
        else { return NULL; }

        if (lhs->GetNodeType() == AST_NODE_CONST_INT && rhs->GetNodeType() == AST_NODE_CONST_INT) {
            if (binary->op == "%") { ret_v = (float)((int)lhs_v % (int)rhs_v); }
            else if (binary->op == "<<") { ret_v = (float)((int)lhs_v << (int)rhs_v); }
            else if (binary->op == ">>") { ret_v = (float)((int)lhs_v >> (int)rhs_v); }
            else if (binary->op == "&") { ret_v = (float)((int)lhs_v & (int)rhs_v); }
            else if (binary->op == "^") { ret_v = (float)((int)lhs_v ^ (int)rhs_v); }
            else if (binary->op == "|") { ret_v = (float)((int)lhs_v | (int)rhs_v); }
        }

        if (binary->op == "*") { ret_v = lhs_v * rhs_v; }
        else if (binary->op == "/") { ret_v = lhs_v / rhs_v; }
        else if (binary->op == "+") { ret_v = lhs_v + rhs_v; }
        else if (binary->op == "-") { ret_v = lhs_v - rhs_v; }
        else if (binary->op == "<") { return new ASTNodeConstInt((int)(lhs_v < rhs_v)); }
        else if (binary->op == ">") {  return new ASTNodeConstInt((int)(lhs_v > rhs_v)); }
        else if (binary->op == "<=") { return new ASTNodeConstInt((int)(lhs_v <= rhs_v)); }
        else if (binary->op == ">=") { return new ASTNodeConstInt((int)(lhs_v >= rhs_v)); }
        else if (binary->op == "==") { return new ASTNodeConstInt((int)(lhs_v == rhs_v)); }
        else if (binary->op == "!=") { return new ASTNodeConstInt((int)(lhs_v != rhs_v)); }
        else if (binary->op == "&&") { return new ASTNodeConstInt((int)(lhs_v && rhs_v)); }
        else if (binary->op == "^^") { return new ASTNodeConstInt((int)(lhs_v || rhs_v && lhs_v != rhs_v)); }
        else if (binary->op == "||") { return new ASTNodeConstInt((int)(lhs_v || rhs_v)); }
        else { return NULL; }

        if (lhs->GetNodeType() == AST_NODE_CONST_FLOAT || rhs->GetNodeType() == AST_NODE_CONST_FLOAT) { return new ASTNodeConstFloat(ret_v); }
        else { return new ASTNodeConstInt((int)std::round(ret_v)); }
    }
    else { return NULL; }
    return NULL;
}

std::string Starsurge::GLSL::Parser::EvaluateType(ASTNodeExpression * expr, ASTNode * scope) {
    if (!expr) { return ""; }
    else if (expr->GetNodeType() == AST_NODE_CONST_INT) { if (((ASTNodeConstInt*)expr)->value < 0) { return "int"; } else { return "uint"; } }
    else if (expr->GetNodeType() == AST_NODE_CONST_FLOAT) { return "float"; }
    else if (expr->GetNodeType() == AST_NODE_CONST_BOOL) { return "bool"; }
    else if (expr->GetNodeType() == AST_NODE_VARIABLE) {
        ASTNodeVariable * cast = (ASTNodeVariable*)expr;
        ASTNodeVariableDeclaration * var = FindVariable(cast->name, scope);
        if (var) {
            std::string ret = var->type;
            if (var->arraySize > 0) ret += "["+std::to_string(var->arraySize)+"]";
            return ret;
        }
        else { return ""; }
    }
    else if (expr->GetNodeType() == AST_NODE_PREFIX_OP) {
        ASTNodePrefixOp * prefix = (ASTNodePrefixOp*)expr;
        std::string type = EvaluateType((ASTNodeExpression*)prefix->GetChild(0), scope);
        if (type == "uint") { return "int"; }
        else if (type == "bool") { return "int"; }
        else if (type == "float" || type == "int") { return type; }
        else { return type; }
    }
    else if (expr->GetNodeType() == AST_NODE_POSTFIX_OP) {
        ASTNodePostfixOp * postfix = (ASTNodePostfixOp*)expr;
        if (postfix->op == "[]") {
            std::string arrayType = EvaluateType((ASTNodeExpression*)postfix->GetChild(0), scope);
            if (arrayType == "mat2" || arrayType == "mat2x2" || arrayType == "mat3x2" || arrayType == "mat4x2") {
                return "vec2";
            }
            else if (arrayType == "mat3" || arrayType == "mat2x3" || arrayType == "mat3x3" || arrayType == "mat4x3") {
                return "vec3";
            }
            else if (arrayType == "mat4" || arrayType == "mat2x4" || arrayType == "mat3x4" || arrayType == "mat4x4") {
                return "vec4";
            }
            else if (arrayType == "vec2" || arrayType == "vec3" || arrayType == "vec4" || arrayType == "color") {
                return "float";
            }
            else if (arrayType == "ivec2" || arrayType == "ivec3" || arrayType == "ivec4") {
                return "int";
            }
            else if (arrayType == "uvec2" || arrayType == "uvec3" || arrayType == "uvec4") {
                return "uint";
            }
            else if (arrayType == "bvec2" || arrayType == "bvec3" || arrayType == "bvec4") {
                return "bool";
            }
            std::size_t bracket_pos = arrayType.find("[");
            if (bracket_pos == std::string::npos) { return ""; }
            return arrayType.substr(0, bracket_pos);
        }
        else {
            std::string type = EvaluateType((ASTNodeExpression*)postfix->GetChild(0), scope);
            if (type == "bool") { return "int"; }
            else if (type == "float" || type == "int" || type == "uint") { return type; }
            else { return type; }
        }
    }
    else if (expr->GetNodeType() == AST_NODE_PAREN_EXPRESSION) {
        return EvaluateType((ASTNodeExpression*)expr->GetChild(0), scope);
    }
    else if (expr->GetNodeType() == AST_NODE_TERNARY) {
        ASTNodeTernary * ternary = (ASTNodeTernary*)expr;
        return EvaluateType(ternary->GetTrue(), scope);
    }
    else if (expr->GetNodeType() == AST_NODE_FUNCTION_CALL) {
        ASTNodeFunctionCall * functionCall = (ASTNodeFunctionCall*)expr;
        std::string f = functionCall->function;
        if (ElemOf<const char*>(BUILTIN_FUNCTIONS, BUILTIN_FUNCTIONS_COUNT, f.c_str())) {
            if (f == "radians" || f == "degrees" || f == "sin" || f == "cos" || f == "tan" || f == "asin" || f == "acos" ||
                f == "atan" || f == "sinh" || f == "cosh" || f == "tanh" || f == "asinh" || f == "acosh" || f == "atanh" ||
                f == "pow" || f == "exp" || f == "log" || f == "exp2" || f == "log2" || f == "sqrt" || f == "inversesqrt" ||
                f == "abs" || f == "sign" || f == "floor" || f == "trunc" || f == "round" || f == "roundEven" || f == "ceil" ||
                f == "fract" || f == "mod" || f == "modf" || f == "min" || f == "max" || f == "clamp" || f == "normalize" ||
                f == "faceforward" || f == "reflect" || f == "refract" || f == "inverse" || f == "mix") {
                return EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
            }
            else if (f == "step") {
                return EvaluateType((ASTNodeExpression*)functionCall->GetChild(1), scope);
            }
            else if (f == "smoothstep") {
                return EvaluateType((ASTNodeExpression*)functionCall->GetChild(2), scope);
            }
            else if (f == "floatBitsToInt") {
                std::string param0type = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                if (param0type == "float") { return "float"; }
                else if (param0type == "vec2") { return "ivec2"; }
                else if (param0type == "vec3") { return "ivec3"; }
                else if (param0type == "vec4") { return "ivec4"; }
                else { return ""; }
            }
            else if (f == "intBitsToFloat") {
                std::string param0type = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                if (param0type == "int") { return "float"; }
                else if (param0type == "ivec2") { return "vec2"; }
                else if (param0type == "ivec3") { return "vec3"; }
                else if (param0type == "ivec4") { return "vec4"; }
                else { return ""; }
            }
            else if (f == "floatBitsToUint") {
                std::string param0type = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                if (param0type == "float") { return "uint"; }
                else if (param0type == "vec2") { return "uvec2"; }
                else if (param0type == "vec3") { return "uvec3"; }
                else if (param0type == "vec4") { return "uvec4"; }
                else { return ""; }
            }
            else if (f == "uintBitsToFloat") {
                std::string param0type = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                if (param0type == "uint") { return "float"; }
                else if (param0type == "uvec2") { return "vec2"; }
                else if (param0type == "uvec3") { return "vec3"; }
                else if (param0type == "uvec4") { return "vec4"; }
                else { return ""; }
            }
            else if (f == "isnan" || f == "isinf" || f == "lessThan" || f == "lessThanEqual" || f == "greaterThan" || f == "greaterThanEqual" ||
                    f == "equal" || f == "notEqual") {
                std::string param0type = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                if (param0type == "float") { return "bool"; }
                else if (param0type == "vec2") { return "bvec2"; }
                else if (param0type == "vec3") { return "bvec3"; }
                else if (param0type == "vec4") { return "bvec4"; }
            }
            else if (f == "any" || f == "all" || f == "not") {
                return "bool";
            }
            else if (f == "length" || f == "distance" || f == "dot" || f == "determinant") {
                return "float";
            }
            else if (f == "cross") {
                return "vec3";
            }
            else if (f == "matrixCompMult") { // Is this correct?
                return EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
            }
            else if (f == "outerProduct") {
                std::string c = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                std::string r = EvaluateType((ASTNodeExpression*)functionCall->GetChild(1), scope);
                if (c == "vec2" && r == "vec2") { return "mat2"; }
                else if (c == "vec3" && r == "vec3") { return "mat3"; }
                else if (c == "vec4" && r == "vec4") { return "mat4"; }
                else if ((c == "vec2" || c == "vec3" || c == "vec4") && (r == "vec2" || r == "vec3" || r == "vec4")) {
                    std::string nrows = r.substr(3);
                    std::string ncols = c.substr(3);
                    return "mat"+nrows+"x"+ncols;
                }
                else { return ""; }
            }
            else if (f == "transpose") {
                std::string m = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                if (m == "mat2" || m == "mat3" || m == "mat4") { return m; }
                else if (m == "mat3x2" || m == "mat2x3" || m == "mat4x2" || m == "mat2x4" || m == "mat4x3" || m == "mat3x4") {
                    std::string nrows = m.substr(3,1);
                    std::string ncols = m.substr(5);
                    return "mat"+ncols+"x"+nrows;
                }
                else { return ""; }
            }
            else if (f == "textureSize") {
                std::string sampler = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                if (sampler == "sampler1D") { return "int"; }
                else if (sampler == "sampler2D") { return "ivec2"; }
                else if (sampler == "sampler3D") { return "ivec3"; }
                else if (sampler == "samplerCube") { return "ivec2"; }
                else if (sampler == "sampler1DShadow") { return "int"; }
                else if (sampler == "sampler2DShadow") { return "ivec2"; }
                else if (sampler == "samplerCubeShadow") { return "ivec2"; }
                else if (sampler == "sampler1DArray") { return "ivec2"; }
                else if (sampler == "sampler2DArray") { return "ivec3"; }
                else if (sampler == "sampler2DRect") { return "ivec2"; }
                else if (sampler == "sampler2DRectShadow") { return "ivec2"; }
                else if (sampler == "sampler1DArray") { return "ivec2"; }
                else if (sampler == "sampler2DArray") { return "ivec3"; }
                else if (sampler == "sampler1DArrayShadow") { return "ivec2"; }
                else if (sampler == "sampler2DArrayShadow") { return "ivec3"; }
                else if (sampler == "sampler2DMS") { return "ivec2"; }
                else if (sampler == "sampler2DMSArray") { return "ivec2"; }
                else { return ""; }
            }
            else if (f == "texture" || f == "textureLod" || f == "textureOffset" || f == "textureProjOffset" ||
                    f == "textureLodOffset" || f == "textureProjLod" || f == "textureProjLodOffset" ||
                    f == "textureGrad" || f == "textureGradOffset" || f == "textureProjGrad" || f == "textureProjGradOffset") {
                std::string sampler = EvaluateType((ASTNodeExpression*)functionCall->GetChild(0), scope);
                if (sampler == "sampler1DShadow" || sampler == "sampler2DShadow" || sampler == "samplerCubeShadow" ||
                    sampler == "sampler1DArrayShadow" || sampler == "sampler2DArrayShadow" || sampler == "sampler2DRectShadow") {
                        return "float";
                }
                else { return "gvec4"; }
            }
            else if (f == "texelFetch" || f == "texelFetchOffset") {
                return "gvec4";
            }
        }
        else {
            std::vector<ASTNodeFunction*> matches = FindFunction(f, scope);
            if (matches.size() > 0) {
                std::vector<std::string> params;
                for (unsigned int i = 0; i < functionCall->ChildCount(); ++i) {
                    params.push_back(EvaluateType((ASTNodeExpression*)functionCall->GetChild(i), scope));
                }

                for (unsigned int i = 0; i < matches.size(); ++i) {
                    if (matches[i]->parameters.size() == params.size()) {
                        bool good = true;
                        for (unsigned int j = 0; j < matches[i]->parameters.size(); ++j) {
                            if (matches[i]->parameters[j].type != params[j]) {
                                good = false;
                                break;
                            }
                        }

                        if (good) {
                            return matches[i]->returnType;
                        }
                    }
                }
                return "";
            }

            // Check constructors.
            std::string constructorType = f;
            std::size_t bracket_pos = f.find("[");
            if (bracket_pos != std::string::npos) { constructorType = constructorType.substr(0, bracket_pos); }

            ASTNodeStruct * strct = FindStruct(constructorType, scope);
            if (strct || ElemOf<const char*>(TYPES, TYPES_COUNT, constructorType.c_str())) {
                return f;
            }
        }

        return "";
    }
    else if (expr->GetNodeType() == AST_NODE_BINARY) {
        ASTNodeBinary * binary = (ASTNodeBinary*)expr;
        ASTNodeExpression * lhs = (ASTNodeExpression*)binary->GetLeft();
        ASTNodeExpression * rhs = (ASTNodeExpression*)binary->GetRight();
        std::string lhsType = EvaluateType(lhs, scope);
        std::string rhsType = EvaluateType(rhs, scope);

        if (binary->op == "==" || binary->op == "!=") { return "bool"; }
        else if (binary->op == "<" || binary->op == ">" || binary->op == ">=" || binary->op == "<=" || binary->op == "&&" ||
                binary->op == "^^" || binary->op == "||") {
            if ((lhsType != "int" && lhsType != "uint" && lhsType != "float" && lhsType != "bool") ||
                (rhsType != "int" && rhsType != "uint" && rhsType != "float" && rhsType != "bool")) { return ""; }
            else { return "bool"; }
        }
        else if ((binary->op == "+" || binary->op == "-" || binary->op == "/") &&
                (lhsType.substr(0,3) == "mat" || rhsType.substr(0,3) == "mat")) { // mat3 + mat3x3
            if (CompatibleTypes(lhsType, rhsType)) {
                return lhsType;
            }
            else { return ""; }
        }
        else if (binary->op == "*" && lhsType.substr(0,3) == "mat" && rhsType.substr(0,3) == "mat") { //mat3 * mat3
            unsigned int lncols = 0;
            unsigned int lnrows = 0;
            if (lhsType == "mat2" || lhsType == "mat3" || lhsType == "mat4") { lncols = std::stoi(lhsType.substr(3)); lnrows = lncols; }
            else {
                lncols = std::stoi(lhsType.substr(3,1));
                lnrows = std::stoi(lhsType.substr(5));
            }

            unsigned int rncols = 0;
            unsigned int rnrows = 0;
            if (rhsType == "mat2" || rhsType == "mat3" || rhsType == "mat4") { rncols = std::stoi(rhsType.substr(3)); rnrows = rncols; }
            else {
                rncols = std::stoi(rhsType.substr(3,1));
                rnrows = std::stoi(rhsType.substr(5));
            }

            if (lncols == rnrows) { return "mat"+std::to_string(rncols)+"x"+std::to_string(lnrows); }
            else { return ""; }
        }
        else if (binary->op == "*" && lhsType.substr(0,3) == "mat" && (rhsType.substr(0,3) == "vec" || rhsType == "color")) { //mat3 * vec3
            unsigned int lncols = 0;
            unsigned int lnrows = 0;
            if (lhsType == "mat2" || lhsType == "mat3" || lhsType == "mat4") { lncols = std::stoi(lhsType.substr(3)); lnrows = lncols; }
            else {
                lncols = std::stoi(lhsType.substr(3,1));
                lnrows = std::stoi(lhsType.substr(5));
            }

            unsigned int rnrows = 0;
            if (rhsType == "color") { rnrows = 4; }
            else { rnrows = std::stoi(rhsType.substr(3)); }

            if (lncols == rnrows) { return "vec"+std::to_string(lnrows); }
            else { return ""; }
        }
        else if (binary->op == "*" || binary->op == "/" || binary->op == "+" || binary->op == "-" ||
                binary->op == "*=" || binary->op == "/=" || binary->op == "+=" || binary->op == "-=") {
            if (lhsType.find("[") != std::string::npos) { // lightIntensity[3] * 3.0f;
                std::string lhsBaseType = lhsType.substr(0, lhsType.find("["));
                if (CompatibleTypes(lhsBaseType, rhsType)) {
                    return lhsType;
                }
            }
            else if (rhsType.find("[") != std::string::npos) { // 3.0f * lightIntensity[3];
                std::string rhsBaseType = rhsType.substr(0, rhsType.find("["));
                if (CompatibleTypes(lhsType, rhsBaseType)) {
                    return rhsType;
                }
            }
            else if (lhsType == "vec2" || lhsType == "vec3" || lhsType == "vec4" || lhsType == "color" ||
                lhsType == "bvec2" || lhsType == "bvec3" || lhsType == "bvec4" ||
                lhsType == "ivec2" || lhsType == "ivec3" || lhsType == "ivec4" ||
                lhsType == "uvec2" || lhsType == "uvec3" || lhsType == "uvec4") { // genType *= genType | genType *= float
                std::string baseType = "float";
                if (lhsType[0] == 'b') { baseType = "bool"; }
                if (lhsType[0] == 'i') { baseType = "int"; }
                if (lhsType[0] == 'u') { baseType = "uint"; }

                if (CompatibleTypes(lhsType, rhsType) || CompatibleTypes(baseType, rhsType)) {
                    return lhsType;
                }
                else { return ""; }
            }
            else if ((rhsType == "vec2" || rhsType == "vec3" || rhsType == "vec4" || rhsType == "color" ||
                rhsType == "bvec2" || rhsType == "bvec3" || rhsType == "bvec4" ||
                rhsType == "ivec2" || rhsType == "ivec3" || rhsType == "ivec4" ||
                rhsType == "uvec2" || rhsType == "uvec3" || rhsType == "uvec4") &&
                (binary->op == "*" || binary->op == "/" || binary->op == "+" || binary->op == "-")) { // genType * genType | float * genType
                std::string baseType = "float";
                if (rhsType[0] == 'b') { baseType = "bool"; }
                if (rhsType[0] == 'i') { baseType = "int"; }
                if (rhsType[0] == 'u') { baseType = "uint"; }

                if (CompatibleTypes(lhsType, rhsType) || CompatibleTypes(lhsType, baseType)) {
                    return rhsType;
                }
                else { return ""; }
            }
            else if ((lhsType != "int" && lhsType != "uint" && lhsType != "float" && lhsType != "bool") ||
                (rhsType != "int" && rhsType != "uint" && rhsType != "float" && rhsType != "bool")) { return ""; }
            else if (lhsType == "float" || rhsType == "float") { return "float"; }
            else if (lhsType == "int" || rhsType == "int") { return "int"; }
            else if (lhsType == "uint" || rhsType == "uint") { return "uint"; }
            else if (lhsType == "bool" || rhsType == "bool") { return "uint"; }
            else { return ""; }
        }
        else if (binary->op == "=") {
            if ((lhsType != "int" && lhsType != "uint" && lhsType != "float" && lhsType != "bool") ||
                (rhsType != "int" && rhsType != "uint" && rhsType != "float" && rhsType != "bool")) {
                if (CompatibleTypes(lhsType, rhsType)) {
                    return lhsType;
                }
                else { return ""; }
            }
            else if (lhsType == "float" || rhsType == "float") { return "float"; }
            else if (lhsType == "int" || rhsType == "int") { return "int"; }
            else if (lhsType == "uint" || rhsType == "uint") { return "uint"; }
            else if (lhsType == "bool" || rhsType == "bool") { return "uint"; }
            else { return ""; }
        }
        else if (binary->op == "%" || binary->op == "<<" || binary->op == ">>" || binary->op == "&" || binary->op == "^" || binary->op == "|" ||
                binary->op == "%=" || binary->op == "<<=" || binary->op == ">>=" || binary->op == "&=" || binary->op == "^=" || binary->op == "|=") {
            if ((lhsType != "int" && lhsType != "uint" && lhsType != "bool") ||
                (rhsType != "int" && rhsType != "uint" && rhsType != "bool")) { return ""; }
            else if (lhsType == "int" || rhsType == "int") { return "int"; }
            else if (lhsType == "uint" || rhsType == "uint") { return "uint"; }
            else if (lhsType == "bool" || rhsType == "bool") { return "uint"; }
            else { return ""; }
        }
        else if (binary->op == ".") {
            if (lhsType.find("[")) { //array[].length()
                if (rhs->GetNodeType() == AST_NODE_FUNCTION_CALL) {
                    if (((ASTNodeFunctionCall*)rhs)->function == "length") {
                        return "uint";
                    }
                    else { return ""; }
                }
            }

            std::string id = ((ASTNodeIdentifier*)rhs)->id;
            if (lhsType == "vec2" || lhsType == "vec3" || lhsType == "vec4" || lhsType == "color" ||
                lhsType == "bvec2" || lhsType == "bvec3" || lhsType == "bvec4" ||
                lhsType == "ivec2" || lhsType == "ivec3" || lhsType == "ivec4" ||
                lhsType == "uvec2" || lhsType == "uvec3" || lhsType == "uvec4") {
                std::string prefix = "";
                if (lhsType[0] != 'v' && lhsType[0] != 'c') { prefix += lhsType[0]; }
                if (id.length() == 4 && lhsType == "color") { return "color"; }
                else if (id.length() == 4) { return prefix+"vec4"; }
                else if (id.length() == 3) { return prefix+"vec3"; }
                else if (id.length() == 2) { return prefix+"vec2"; }
                else if (id.length() == 1) {
                    if (prefix == "b") { return "bool"; }
                    if (prefix == "i") { return "int"; }
                    if (prefix == "u") { return "uint"; }
                    else { return "float"; }
                }
                else { return ""; }
            }
            else if (lhsType != "") {
                ASTNodeStruct * strct = FindStruct(lhsType, scope);
                if (strct) {
                    // Find field 'id'
                    std::vector<ASTNodeField*> fields = strct->GetChildren<ASTNodeField>(AST_NODE_FIELD);
                    for (unsigned int i = 0; i < fields.size(); ++i) {
                        if (fields[i]->name == id) {
                            std::string ret = fields[i]->type;
                            if (fields[i]->arraySize > 0) ret += "["+std::to_string(fields[i]->arraySize)+"]";
                            return ret;
                        }
                    }
                    return "";
                }
            }
            else { return ""; }
        }

    }
    else { return ""; }
    return "";
}

Starsurge::GLSL::ASTNodeProgram * Starsurge::GLSL::Parser::Program() {
    ASTNodeProgram * node = new ASTNodeProgram();

    // {Statement}
    while (!AtEnd()) {
        ASTNodeStatement* statement = Statement(node);
        if (statement == NULL) {
            return node;
        }
        node->AddChild(statement);
    }
    return node;
}

Starsurge::GLSL::ASTNodeStatement * Starsurge::GLSL::Parser::Statement(ASTNode * scope) {
    unsigned int start_position = Tell();

    // 'for' '('[VariableDeclaration | Expression] ';' [Expression] ';' [Expression] ')' Scope
    if (Match(TOKEN_FOR) && Match(TOKEN_LEFT_PAREN)) {
        // We need to preattach the node for scoping.
        ASTNodeFor * ret = new ASTNodeFor();
        scope->AddChild(ret);
        ASTNodeScope * newScope = new ASTNodeScope();
        ret->SetScope(newScope);

        //[VariableDeclaration | Expression]
        ASTNode * declares = VariableDeclaration(newScope);
        if (declares) {
            ret->SetDeclares(declares);
            if (declares->ChildCount() == 1) { // Simplify the tree some.
                ret->SetDeclares(declares->GetChild(0));
            }
        }
        else {
            ASTNodeExpression * expr = Expression(newScope);
            if (expr) {
                ret->SetDeclares(expr);
            }
        }

        // ';'
        if (Match(TOKEN_SEMICOLON)) {
            // [Expression]
            ASTNodeExpression * condition = Expression(newScope);
            if (condition) {
                std::string conditionType = EvaluateType(condition, scope);
                if (conditionType != "" && !CompatibleTypes("bool", conditionType)) {
                    Error("Condition in for statement should be of bool type.");
                    return NULL;
                }
                ret->SetCondition(condition);
            }

            // ';'
            if (Match(TOKEN_SEMICOLON)) {
                // [Expression]
                ASTNodeExpression * expr2 = Expression(newScope);
                if (expr2) {
                    ret->SetExpression(expr2);
                }

                scope->RemoveChild(ret);

                // ')' Scope
                if (Match(TOKEN_RIGHT_PAREN)) {
                    if (Scope(scope, ret, newScope)) {
                        return ret;
                    }
                    else {
                        Error("Expected '{'.");
                        return NULL;
                    }
                }
                else {
                    Error("Expected ')'.");
                    return NULL;
                }
            }
            else {
                Error("Expected ';' inside for loop.");
                return NULL;
            }
        }
        else {
            Error("Expected ';' inside for loop.");
            return NULL;
        }
    }
    Seek(start_position);

    // 'while' '(' Expression ')' TOKEN_SCOPE_BEGIN
    if (Match(TOKEN_WHILE)) {
        if (Match(TOKEN_LEFT_PAREN)) {
            ASTNodeWhile * ret = new ASTNodeWhile();
            ASTNodeExpression * expr = Expression(scope);
            std::string conditionType = EvaluateType(expr, scope);
            if (conditionType != "" && !CompatibleTypes("bool", conditionType)) {
                Error("Condition in while statement should be of bool type.");
                return NULL;
            }
            if (expr) {
                if (Match(TOKEN_RIGHT_PAREN)) {
                    ret->SetCondition(expr);

                    ASTNodeScope * newScope = new ASTNodeScope();
                    ret->SetScope(newScope);
                    if (Scope(scope, ret, newScope)) {
                        return ret;
                    }
                    else {
                        Error("Expected '{' after condition.");
                        return NULL;
                    }
                }
                else {
                    Error("Expected ')' after expression.");
                    return NULL;
                }
            }
            else {
                Error("Inalid condition for while loop.");
                return NULL;
            }
        }
        else {
            Error("Expected '(' after 'while' keyword.");
            return NULL;
        }
    }
    Seek(start_position);

    // 'do' (Scope | FunctionCall ';') 'while' '(' Expression ')' ';'
    if (Match(TOKEN_DO)) {
        ASTNodeDo * ret = new ASTNodeDo();
        ASTNodeScope * newScope = new ASTNodeScope();
        ret->SetScope(newScope);
        if (!Scope(scope, ret, newScope)) {
            ASTNodeFunctionCall * functionCall = FunctionCall(scope);
            newScope->AddChild(functionCall);
            if (!functionCall) {
                Error("Unexpected token after 'do' keyword.");
                return NULL;
            }
            else if (functionCall && !Match(TOKEN_SEMICOLON)) {
                Error("Expected ';'.");
                return NULL;
            }
        }

        if (!Match(TOKEN_WHILE)) {
            Error("Expected 'while' keyword.'");
            return NULL;
        }
        if (!Match(TOKEN_LEFT_PAREN)) {
            Error("Expected '(' after 'while' keyword.'");
            return NULL;
        }
        ASTNodeExpression * expr = Expression(scope);
        std::string conditionType = EvaluateType(expr, scope);
        if (conditionType != "" && !CompatibleTypes("bool", conditionType)) {
            Error("Condition in do-while statement should be of bool type.");
            return NULL;
        }
        if (expr) {
            if (Match(TOKEN_RIGHT_PAREN)) {
                if (!Match(TOKEN_SEMICOLON)) {
                    Error("Expected ';' after expression.");
                    return NULL;
                }
                ret->SetCondition(expr);
                return ret;
            }
            else {
                Error("Expected ')' after expression.");
                return NULL;
            }
        }
        return NULL;
    }
    Seek(start_position);

    // 'switch' '(' Expression ')' '{' {Case} '}'
    if (Match(TOKEN_SWITCH)) {
        if (Match(TOKEN_LEFT_PAREN)) {
            ASTNodeSwitch * ret = new ASTNodeSwitch();
            ASTNodeExpression * expr = Expression(scope);
            if (expr) {
                ret->SetInput(expr);
                if (Match(TOKEN_RIGHT_PAREN)) {
                    if (Match(TOKEN_SCOPE_BEGIN)) {
                        scope->AddChild(ret);
                        while (Check(TOKEN_DEFAULT) || Check(TOKEN_CASE)) {
                            ASTNodeCase * node = Case(scope, ret);
                            if (!node) {
                                scope->RemoveChild(ret);
                                return NULL;
                            }
                            ret->AddCase(node);
                        }
                        scope->RemoveChild(ret);

                        if (Match(TOKEN_SCOPE_END)) {
                            return ret;
                        }
                        else {
                            Error("Expected '}'.");
                            return NULL;
                        }
                    }
                    else {
                        Error("Expected ')' after expression.");
                        return NULL;
                    }
                }
                else {
                    Error("Expected ')' after expression.");
                    return NULL;
                }
            }
            else {
                Error("Invalid input for switch.");
                return NULL;
            }
        }
        else {
            Error("Expected '(' after 'switch' keyword.");
            return NULL;
        }
    }

    // {QUALIFIER} Struct [{IDENTIFIER ['[' INT ']'] ','} IDENTIFIER ['[' INT ']']] ';'
    QualifierData qualData = Qualifiers();
    ASTNodeStruct * strct = Struct(scope);
    if (strct) {
        if (Match(TOKEN_SEMICOLON)) {
            if (qualData.storage != "" || qualData.misc != "" || qualData.interp != "" || qualData.precision != "" || qualData.memory != ""
                || qualData.invariant) { // This is not a variable decalaration
                Error("Unexpected quantifiers before struct.");
                return NULL;
            }
            return strct;
        }
        else {
            ASTNode * container = new ASTNode();
            while (true) {
                if (!Match(TOKEN_IDENTIFIER)) { Error("Unexpected identifier '"+Previous().data+"' for struct."); return NULL; }
                ASTNodeVariableDeclaration * var = new ASTNodeVariableDeclaration();
                var->name = Previous().data;
                var->type = strct->name;
                var->storage = qualData.storage;
                var->misc = qualData.misc;
                var->interp = qualData.interp;
                var->precision = qualData.precision;
                var->memory = qualData.memory;
                var->isInvariant = qualData.invariant;

                var->arraySize = ArrayDeclare(scope);
                if (var->arraySize < 0) { return NULL; }

                container->AddChild(var);
                if (!Match(TOKEN_OP_COMMA)) {
                    break;
                }
            }

            if (Match(TOKEN_SEMICOLON)) {
                // Everything was fine, add everything to the tree.
                scope->AddChild(strct);
                std::vector<ASTNode*> variables = container->GetAllChildren();
                for (unsigned int i = 0; i < variables.size()-1; ++i) {
                    scope->AddChild(variables[i]);
                }
                return (ASTNodeVariableDeclaration*)variables[variables.size()-1];
            }
            else {
                Error("Expected ';'.");
                return NULL;
            }
        }
    }
    Seek(start_position);

    // 'if' '(' Expression ')' (Scope | Statement) {'else' 'if' '(' Expression ')' (Scope | Statement)} ['else' (Scope | Statement)]
    if (Match(TOKEN_IF)) {
        if (!Match(TOKEN_LEFT_PAREN)) {
            Error("Expected '(' after 'if' keyword.");
            return NULL;
        }
        ASTNodeExpression * condition = Expression(scope);
        std::string conditionType = EvaluateType(condition, scope);
        if (condition == NULL || (conditionType != "" && !CompatibleTypes("bool", conditionType))) {
            Error("Expression in if statement should be of bool type.");
            return NULL;
        }
        if (!Match(TOKEN_RIGHT_PAREN)) {
            Error("Expected ')' after boolean expression in if statement.");
            return NULL;
        }
        ASTNodeIf * ret = new ASTNodeIf();
        ASTNodeScope * newScope = new ASTNodeScope();
        ret->SetCondition(condition);
        ret->SetScope(newScope);
        scope->AddChild(ret);
        if (Check(TOKEN_SCOPE_BEGIN)) {
            if (!Scope(scope, ret, newScope)) {
                scope->RemoveChild(ret);
                return NULL;
            }
        }
        else {
            ASTNodeStatement * stmt = Statement(scope);
            if (stmt == NULL) {
                scope->RemoveChild(ret);
                return NULL;
            }
            newScope->AddChild(stmt);
        }

        ASTNodeIf * lastIf = ret;
        while (Match(TOKEN_ELSE)) {
            ASTNodeIf * elseif = new ASTNodeIf();
            ASTNodeScope * elseifScope = new ASTNodeScope();
            elseif->SetScope(elseifScope);
            lastIf->SetNext(elseif);
            if (Match(TOKEN_IF)) {
                if (!Match(TOKEN_LEFT_PAREN)) {
                    Error("Expected '(' after 'if' keyword.");
                    scope->RemoveChild(ret);
                    return NULL;
                }
                ASTNodeExpression * condition = Expression(scope);
                std::string conditionType = EvaluateType(condition, scope);
                if (condition == NULL || (conditionType != "" && !CompatibleTypes("bool", conditionType))) {
                    Error("Expression in if statement should be of bool type.");
                    scope->RemoveChild(ret);
                    return NULL;
                }
                if (!Match(TOKEN_RIGHT_PAREN)) {
                    Error("Expected ')' after boolean expression in if statement.");
                    scope->RemoveChild(ret);
                    return NULL;
                }
                elseif->SetCondition(condition);
            }

            if (Check(TOKEN_SCOPE_BEGIN)) {
                if (!Scope(scope, elseif, elseifScope)) {
                    scope->RemoveChild(ret);
                    scope->RemoveChild(elseif);
                    return NULL;
                }
                scope->RemoveChild(elseif);
                lastIf->SetNext(elseif);
            }
            else {
                ASTNodeStatement * stmt = Statement(scope);
                if (stmt == NULL) {
                    scope->RemoveChild(ret);
                    return NULL;
                }
                elseifScope->AddChild(stmt);
            }

            lastIf = elseif;
        }

        scope->RemoveChild(ret);
        return ret;
    }
    Seek(start_position);

    // ('continue' | 'break' | 'return' [Expression] | 'discard') ';'
    if (Match({TOKEN_CONTINUE, TOKEN_BREAK, TOKEN_RETURN, TOKEN_DISCARD})) {
        Token keyword = Previous();
        ASTNodeJump * ret = new ASTNodeJump(keyword.data);
        if (keyword.type == TOKEN_CONTINUE) { // Continue can only be used in for, while and do-while loops.
            ASTNode * scopeParent = scope->GetParent();
            if (scopeParent == NULL || (scopeParent->GetNodeType() != AST_NODE_FOR && scopeParent->GetNodeType() != AST_NODE_WHILE &&
                scopeParent->GetNodeType() != AST_NODE_DO)) {
                Error("The keyword 'continue' is only available in for, while and do-while loops.");
                return NULL;
            }
        }
        else if (keyword.type == TOKEN_BREAK) { // Break can only be used in for, while and do-while loops as well as switch
            ASTNode * scopeParent = scope->GetParent();
            if (scopeParent == NULL || (scopeParent->GetNodeType() != AST_NODE_FOR && scopeParent->GetNodeType() != AST_NODE_WHILE &&
                scopeParent->GetNodeType() != AST_NODE_DO || scopeParent->GetNodeType() != AST_NODE_CASE)) {
                Error("The keyword 'break' is only available in loops or switches.");
                return NULL;
            }
            if (scopeParent->GetNodeType() == AST_NODE_CASE) { return NULL; }
        }
        else if (keyword.type == TOKEN_DISCARD) { // Can only be used in the function 'fragment()'.
            if (scope->GetNodeType() != AST_NODE_FUNCTION || ((ASTNodeFunction*)scope)->name != "fragment") {
                Error("The keyword 'discard' is only available 'fragment()'.");
                return NULL;
            }
        }
        else if (keyword.type == TOKEN_RETURN) {
            if (!Check(TOKEN_SEMICOLON)) {
                ASTNodeExpression * expr = Expression(scope);
                if (expr) { ret->AddChild(expr); }
                else { return NULL; }
            }
        }

        if (Match(TOKEN_SEMICOLON)) {
            return ret;
        }
        else {
            Error("Expected ';' after '"+keyword.data+"'.");
            return NULL;
        }
    }

    // FunctionDeclaration (';' | Scope)
    ASTNodeFunction * func = FunctionDeclaration(scope);
    if (func) {
        scope->AddChild(func);
        if (Match(TOKEN_SEMICOLON)) {
            return func;
        }
        else if (Check(TOKEN_SCOPE_BEGIN)) {
            ASTNode * newScope = Scope(scope, func, func);
            if (newScope) {
                return func;
            }
        }
        else {
            Error("Expected either ';' or '{'.");
        }
        scope->RemoveChild(func);
    }
    Seek(start_position);

    // VariableDeclaration ';'
    if (PeekVariableDeclaration(scope)) {
        ASTNode * node = VariableDeclaration(scope);
        if (node && node->ChildCount() > 0) {
            if (Match(TOKEN_SEMICOLON)) {
                std::vector<ASTNode*> variables = node->GetAllChildren();
                for (unsigned int i = 0; i < variables.size()-1; ++i) {
                    scope->AddChild(variables[i]);
                }
                return (ASTNodeVariableDeclaration*)variables[variables.size()-1];
            }
            else {
                Error("Expected ';' after variable declaration(s).");
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }
    Seek(start_position);

    // Scope
    if (Check(TOKEN_SCOPE_BEGIN)) {
        ASTNode * newScope = Scope(scope, scope);
        if (newScope) {
            return (ASTNodeScope*)newScope;
        }
        else {
            return NULL;
        }
    }
    Seek(start_position);

    // Expression ';'
    ASTNodeExpression * expr = Expression(scope);
    if (expr) {
        if (Match(TOKEN_SEMICOLON)) {
            ASTNodeExpressionStatement * ret = new ASTNodeExpressionStatement();
            ret->AddChild(expr);
            return ret;
        }
        else {
            Error("Expected ';' after expression.");
            return NULL;
        }
    }
    Seek(start_position);

    // ';'
    if (Match(TOKEN_SEMICOLON)) {
        return new ASTNodeEmptyStatement();
    }

    Error("Invalid statement starting with '"+Peek().data+"'.");
    return NULL;
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::Parser::Scope(ASTNode * scope, ASTNode * parent, ASTNode * target) {
    // '{' {Statement} '}'
    if (Match(TOKEN_SCOPE_BEGIN)) {
        if (!target) {
            target = new ASTNodeScope();
        }
        if (!scope->IsChild(parent) && parent != scope) { // Statements need to climb up the tree.
            scope->AddChild(parent);
        }
        if (!parent->IsChild(target) && parent != target) {
            parent->AddChild(target);
        }

        while (!Check(TOKEN_SCOPE_END)) {
            ASTNodeStatement* statement = Statement(target);
            if (statement) {
                target->AddChild(statement);
            }
            else {
                return NULL;
            }
        }

        if (scope != parent) {
            scope->RemoveChild(parent);
        }

        if (Match(TOKEN_SCOPE_END)) {
            return target;
        }
        else {
            Error("Expected '}'.");
        }
    }

    return NULL;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::Parser::Expression(ASTNode * scope, int precedence) {
    // http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
    ASTNodeExpression * lhs = Unary(scope);
    if (!lhs) {
        return NULL;
    }
    if (EvaluateType(lhs, scope) == "") {
        Error("Bad unary expression.");
        return NULL;
    }
    int maxPrecedence = MAX_OPERATOR_PRECEDENCE;

    while (Operator()) {
        bool isAssignment = OperatorAssign();
        Token op = Peek();
        if (!(precedence <= OPERATOR_PRECEDENCE[op.type] && OPERATOR_PRECEDENCE[op.type] <= maxPrecedence)) { break; }
        Advance();

        ASTNodeExpression * rhs = Expression(scope, OPERATOR_PRECEDENCE[op.type]+1);
        if (!rhs || EvaluateType(rhs, scope) == "") {
            Error("Invalid expression to the right of '"+op.data+"'.");
            return NULL;
        }

        ASTNodeBinary * binary = new ASTNodeBinary(op.data);
        binary->SetLeft(lhs);
        binary->SetRight(rhs);
        lhs = binary;

        // Make sure the operation was legal.
        // if (EvaluateType(binary, scope) == "") {
        //     Error("Illegal operation: "+EvaluateType((ASTNodeExpression*)binary->GetLeft(), scope)+op.data+EvaluateType((ASTNodeExpression*)binary->GetRight(), scope));
        //     return NULL;
        // }

        maxPrecedence = OPERATOR_PRECEDENCE[op.type];
        if (isAssignment) { // Right-associative operators
            maxPrecedence = OPERATOR_PRECEDENCE[op.type]-1;
        }
    }

    return lhs;
}

bool Starsurge::GLSL::Parser::PeekVariableDeclaration(ASTNode * scope) {
    unsigned int pos = Tell();
    QualifierData qualData = Qualifiers();
    if (!Type(scope)) {
        Seek(pos);
        return false;
    }

    if (!Match(TOKEN_IDENTIFIER)) {
        Seek(pos);
        return false;
    }

    if (Match(TOKEN_LEFT_PAREN)) { //It's a function declare.
        Seek(pos);
        return false;
    }

    Seek(pos);
    return true;
}

Starsurge::GLSL::ASTNode * Starsurge::GLSL::Parser::VariableDeclaration(ASTNode * scope) {
    // {QUALIFIER} Type {IDENTIFIER ['[' Expression ']'] [ '=' Expression] ','} IDENTIFIER ['[' Expression ']'] ['=' Expression]
    QualifierData qualData = Qualifiers();
    if (!Type(scope)) {
        return NULL;
    }
    std::string type = Previous().data;

    ASTNode * ret = new ASTNode();

    while (true) {
        std::string name = "";
        if (Match(TOKEN_IDENTIFIER)) {
            name = Previous().data;
        }
        else {
            return NULL;
        }

        ASTNodeVariableDeclaration * var = new ASTNodeVariableDeclaration();
        var->name = name;
        var->type = type;
        var->storage = qualData.storage;
        var->misc = qualData.misc;
        var->interp = qualData.interp;
        var->precision = qualData.precision;
        var->memory = qualData.memory;
        var->isInvariant = qualData.invariant;

        if (var->type == "void") {
            Error("Cannot declare a variable with type 'void'.");
            return NULL;
        }

        var->arraySize = ArrayDeclare(scope);
        if (var->arraySize < 0) { return NULL; }

        if (Match(TOKEN_OP_ASSIGN)) {
            Token op = Previous();
            ASTNodeExpression * rhs = Expression(scope);
            if (rhs) {
                std::string rhsType = EvaluateType(rhs, scope);
                std::string lhsType = var->type;
                if (var->arraySize > 0) { lhsType += "[" + std::to_string(var->arraySize) + "]"; }
                if (CompatibleTypes(lhsType, rhsType) || rhsType == "") {
                    var->SetInitialValue(rhs);
                }
                else {
                    Error("Cannot set variable of type '"+lhsType+"' to type '"+rhsType+"'");
                    return NULL;
                }
            }
            else {
                Error("Invalid expression to the right of '='.");
                return NULL;
            }
        }
        else if (var->storage == "const") {
            Error("const-qualified variables must be defined.");
            return NULL;
        }

        ret->AddChild(var);

        if (!Match(TOKEN_OP_COMMA)) {
            break;
        }
    }

    return ret;
}

Starsurge::GLSL::ASTNodeFunction * Starsurge::GLSL::Parser::FunctionDeclaration(ASTNode * scope) {
    // [{QUALIFIER}] Type IDENTIFIER '(' {['in' / 'out' /'inout'] Type IDENTIFIER ','} ['in' / 'out' /'inout'] Type IDENTIFIER ')'
    QualifierData qualData = Qualifiers();
    if (!Type(scope)) {
        return NULL;
    }
    std::string type = Previous().data;

    if (!Match(TOKEN_IDENTIFIER)) {
        return NULL;
    }
    std::string name = Previous().data;

    if (!Match(TOKEN_LEFT_PAREN)) {
        return NULL;
    }

    ASTNodeFunction * ret = new ASTNodeFunction();
    ret->name = name;
    ret->returnType = type;
    ret->storage = qualData.storage;
    ret->misc = qualData.misc;
    ret->interp = qualData.interp;
    ret->precision = qualData.precision;
    ret->memory = qualData.memory;
    ret->isInvariant = qualData.invariant;

    while (!Match(TOKEN_RIGHT_PAREN)) {
        FunctionParameter param;
        if (Match(TOKEN_QUALIFIER)) {
            param.qualifier = Previous().data;

            if (param.qualifier != "in" && param.qualifier != "out" && param.qualifier != "inout") {
                return NULL;
            }
        }

        if (!Type(scope)) {
            Error("Invalid typename '"+Peek().data+"'.");
            return NULL;
        }
        param.type = Previous().data;

        if (!Match(TOKEN_IDENTIFIER)) {
            Error("Expected identifier for parameter name.");
            return NULL;
        }
        param.name = Previous().data;

        ret->parameters.push_back(param);

        if (!(Match(TOKEN_OP_COMMA) || Check(TOKEN_RIGHT_PAREN))) {
            Error("Expected ',' or ')' in function header.");
            return NULL;
        }
    }
    return ret;
}

Starsurge::GLSL::ASTNodeStruct * Starsurge::GLSL::Parser::Struct(ASTNode * scope) {
    // 'struct' IDENTIFIER '{' {FieldDeclaration} '}'
    if (Match(TOKEN_STRUCT)) {
        ASTNodeStruct * ret = new ASTNodeStruct();

        if (!Match(TOKEN_IDENTIFIER)) {
            Error("Expected identifier for struct name.");
            return NULL;
        }
        ret->name = Previous().data;

        if (!Match(TOKEN_SCOPE_BEGIN)) {
            Error("Expected '{'.");
            return NULL;
        }

        while (!Match(TOKEN_SCOPE_END)) {
            if (!FieldDeclaration(scope, ret)) {
                return NULL;
            }
        }

        return ret;
    }

    return NULL;
}

Starsurge::GLSL::ASTNodeCase * Starsurge::GLSL::Parser::Case(ASTNode * scope, ASTNodeSwitch * parent) {
    // ('default' | 'case' Expression) ':' ['{'] {Statement} ['}'] 'break' ';'
    if (Match(TOKEN_DEFAULT) || Match(TOKEN_CASE)) {
        ASTNodeCase * ret = new ASTNodeCase();
        if (Previous().type == TOKEN_DEFAULT) {
            ret->isDefault = true;
        }
        else {
            ASTNodeExpression * expr = Expression(scope);
            ret->SetCondition(expr);
        }

        if (!Match(TOKEN_OP_COLON)) {
            Error("Expected ':' after expression in case.");
            return NULL;
        }

        ASTNodeScope * newScope = new ASTNodeScope();
        ret->SetScope(newScope);
        parent->AddCase(ret);
        if (Check(TOKEN_SCOPE_BEGIN)) {
            if (!Scope(scope, ret, newScope)) {
                parent->RemoveChild(ret);
                return NULL;
            }
        }
        else {
            while (!Check(TOKEN_BREAK)) {
                ASTNodeStatement* statement = Statement(newScope);
                if (statement) {
                    newScope->AddChild(statement);
                }
                else {
                    parent->RemoveChild(ret);
                    return NULL;
                }
            }
        }

        if (!Match(TOKEN_BREAK)) {
            parent->RemoveChild(ret);
            return NULL;
        }

        if (!Match(TOKEN_SEMICOLON)) {
            parent->RemoveChild(ret);
            Error("Expected ';' after 'break' keyword.");
            return NULL;
        }

        return ret;
    }

    return NULL;
}

bool Starsurge::GLSL::Parser::FieldDeclaration(ASTNode * scope, ASTNodeStruct * strct) {
    // TYPE {IDENTIFIER {'[' INT ']'} ','} IDENTIFIER {'[' INT ']'} ';'
    if (!Type(scope)) {
        Error("Invalid typename '"+Peek().data+"'.");
        return false;
    }
    std::string type = Previous().data;

    while (true) {
        ASTNodeField * var = new ASTNodeField();
        if (!Match(TOKEN_IDENTIFIER)) {
            Error("Expected identifier for field name.");
            return false;
        }
        var->name = Previous().data;
        var->type = type;

        var->arraySize = ArrayDeclare(scope);
        if (var->arraySize < 0) { return NULL; }

        strct->AddChild(var);
        if (!Match(TOKEN_OP_COMMA)) {
            break;
        }
    }

    if (!Match(TOKEN_SEMICOLON)) {
        Error("Expected ';'.");
        return false;
    }

    return true;
}

Starsurge::GLSL::QualifierData Starsurge::GLSL::Parser::Qualifiers() {
    QualifierData data;
    data.storage = "";
    data.misc = "";
    data.interp = "";
    data.precision = "";
    data.memory = "";
    data.invariant = false;

    while (Match(TOKEN_QUALIFIER)) {
        Token qualifier = Previous();
        if (Starsurge::ElemOf<const char*>(QUALIFIERS_STORAGE, QUALIFIERS_STORAGE_COUNT, qualifier.data.c_str())) {
            if (data.storage != "") {
                Error("Qualifier '"+qualifier.data+"' overrides '"+data.storage+"'");
                return data;
            }
            data.storage = qualifier.data;
        }
        else if (Starsurge::ElemOf<const char*>(QUALIFIERS_MISC, QUALIFIERS_MISC_COUNT, qualifier.data.c_str())) {
            if (data.misc != "") {
                Error("Qualifier '"+qualifier.data+"' overrides '"+data.misc+"'");
                return data;
            }
            data.misc = qualifier.data;
        }
        else if (Starsurge::ElemOf<const char*>(QUALIFIERS_INTERP, QUALIFIERS_INTERP_COUNT, qualifier.data.c_str())) {
            if (data.interp != "") {
                Error("Qualifier '"+qualifier.data+"' overrides '"+data.interp+"'");
                return data;
            }
            data.interp = qualifier.data;
        }
        else if (Starsurge::ElemOf<const char*>(QUALIFIERS_PREC, QUALIFIERS_PREC_COUNT, qualifier.data.c_str())) {
            if (data.precision != "") {
                Error("Qualifier '"+qualifier.data+"' overrides '"+data.precision+"'");
                return data;
            }
            data.precision = qualifier.data;
        }
        else if (Starsurge::ElemOf<const char*>(QUALIFIERS_MEMORY, QUALIFIERS_MEMORY_COUNT, qualifier.data.c_str())) {
            if (data.memory != "") {
                Error("Qualifier '"+qualifier.data+"' overrides '"+data.memory+"'");
                return data;
            }
            data.memory = qualifier.data;
        }
        else if (strcmp(QUALIFIERS_INVARIANT, qualifier.data.c_str())) {
            if (data.invariant) {
                Error("Qualifier 'invariant' repeated.");
                return data;
            }
            data.invariant = true;
        }
        else {
            Error("Unknown qualifier '"+qualifier.data+"'");
            return data;
        }
    }

    return data;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::Parser::UnaryPrimary(ASTNode * scope) {
    unsigned int oldPosition = Tell();
    // '(' Expression ')'
    if (Match(TOKEN_LEFT_PAREN)) {
        ASTNodeExpression * node = Expression(scope);
        if (!Match(TOKEN_RIGHT_PAREN)) {
            Error("Expected ')' after expression.");
            return NULL;
        }
        ASTNodeParenExpression * ret = new ASTNodeParenExpression();
        ret->AddChild(node);
        return ret;
    }

    // FunctionCall
    ASTNodeFunctionCall * functionCall = FunctionCall(scope);
    if (functionCall) {
        return functionCall;
    }
    Seek(oldPosition);

    // Variable
    if (Variable(scope)) {
        return new ASTNodeVariable(Previous().data);
    }

    // 'true' / 'false'
    if (Match(TOKEN_TRUE) || Match(TOKEN_FALSE)) {
        Token bol = Previous();
        return new ASTNodeConstBool(bol.type == TOKEN_TRUE);
    }

    // INT / FLOAT
    if (Match(TOKEN_INT)) { return new ASTNodeConstInt(std::stoi(Previous().data)); }
    if (Match(TOKEN_FLOAT)) { return new ASTNodeConstFloat(std::stof(Previous().data)); }

    return NULL;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::Parser::UnaryPostfix(ASTNode * scope) {
    // UnaryPrimary
    ASTNodeExpression * postfix = UnaryPrimary(scope);
    if (!postfix) { return NULL; }

    while (true) {
        // UnaryPostfix '[' Expression ']'
        if (Match(TOKEN_LEFT_BRACKET)) {
            ASTNodeExpression * expr = Expression(scope);
            if (expr && Match(TOKEN_RIGHT_BRACKET)) {
                ASTNodePostfixOp * ret = new ASTNodePostfixOp("[]");
                ASTNodeExpression * evaluated = Evaluate(expr, scope); // Try to evaluate to simplify the tree.
                ret->AddChild(postfix);
                if (evaluated) { ret->AddChild(evaluated); }
                else { ret->AddChild(expr); }
                postfix = ret;
            }
            else {
                Error("Expected matching ']'.");
                return NULL;
            }
        }
        // Variable '.' FieldSelectionSwizzling
        else if (Match(TOKEN_OP_DOT)) {
            if (Match(TOKEN_IDENTIFIER)) {
                std::string swizzle = Previous().data;
                std::string type = EvaluateType(postfix, scope);
                if (type.find("[") != std::string::npos && type.find("]") != std::string::npos) {
                    if (swizzle == "length" && Match(TOKEN_LEFT_PAREN)) {
                        if (!Match(TOKEN_RIGHT_PAREN)) {
                            Error("Expected matching ')'.");
                            return NULL;
                        }

                        ASTNodeBinary * ret = new ASTNodeBinary(".");
                        ret->SetLeft(postfix);
                        ret->SetRight(new ASTNodeFunctionCall("length"));
                        postfix = ret;
                        continue;
                    }
                }
                if (ElemOf<const char*>(TYPES, TYPES_COUNT, type.c_str())) {
                    std::string valid_chars = "";
                    if (type == "vec4" || type == "bvec4" || type == "ivec4" || type == "uvec4" || type == "gvec4" || type == "color") {
                        valid_chars="xyzwrgbastpq";
                    }
                    else if (type == "vec3" || type == "bvec3" || type == "ivec3" || type == "uvec3"  || type == "gvec3") {
                        valid_chars = "xyzrgbstp";
                    }
                    else if (type == "vec2" || type == "bvec2" || type == "ivec2" || type == "uvec2" || type == "gvec2") {
                        valid_chars = "xyrgst";
                    }
                    else {
                        Error("Swizzling is only supported for vectors.");
                        return NULL;
                    }

                    for (unsigned int i = 0; i < swizzle.length(); ++i) {
                        bool good = false;
                        for (unsigned int j = 0; j < valid_chars.length(); ++j) {
                            if (swizzle[i] == valid_chars[j]) {
                                good = true;
                                break;
                            }
                        }

                        if (!good) {
                            Error("Invalid character '"+std::to_string(swizzle[i])+"' in swizzle.");
                            return NULL;
                        }
                    }

                    ASTNodeBinary * ret = new ASTNodeBinary(".");
                    ret->SetLeft(postfix);
                    ret->SetRight(new ASTNodeIdentifier(swizzle));
                    postfix = ret;
                }
                else {
                    ASTNodeStruct * strct = FindStruct(type, scope);
                    if (strct) {
                        std::vector<ASTNodeField*> fields = strct->GetChildren<ASTNodeField>(AST_NODE_FIELD);
                        for (unsigned int i = 0; i < fields.size(); ++i) {
                            if (fields[i]->name == swizzle) {
                                ASTNodeBinary * ret = new ASTNodeBinary(".");
                                ret->SetLeft(postfix);
                                ret->SetRight(new ASTNodeIdentifier(swizzle));
                                return ret;
                            }
                        }

                        Error("Unknown field '"+swizzle+"'.");
                        return NULL;
                    }

                    Error("Unknown type '"+type+"'.");
                    return NULL;
                }
            }
            else {
                Error("Unexpected token '"+Peek().data+"' to the right of '.'.");
                return NULL;
            }
        }
        // UnaryPostfix '?' Expression ':' Expression
        else if (Match(TOKEN_OP_QUESTION)) {
            ASTNodeExpression * expr1 = Expression(scope);
            if (!expr1) {
                Error("Bad expression in ternary expression.");
                return NULL;
            }
            if (!Match(TOKEN_OP_COLON)) {
                Error("Expected ':' in ternary expression.");
                return NULL;
            }
            ASTNodeExpression * expr2 = Expression(scope);
            if (!expr2) {
                Error("Bad expression in ternary expression.");
                return NULL;
            }

            std::string trueType = EvaluateType(expr1, scope);
            std::string falseType = EvaluateType(expr2, scope);
            if (!CompatibleTypes(trueType, falseType)) {
                Error("The true statement and false statement of a ternary expression should have compatible types.");
                return NULL;
            }

            ASTNodeTernary * ret = new ASTNodeTernary();
            ret->SetCondition(postfix);
            ret->SetTrue(expr1);
            ret->SetFalse(expr2);
            postfix = ret;
        }
        // UnaryPostfix ('++' / '--')
        else if (Match(TOKEN_OP_INC) || Match(TOKEN_OP_DEC)) {
            Token op = Previous();
            ASTNodePostfixOp * node = new ASTNodePostfixOp(op.data);
            node->AddChild(postfix);
            postfix = node;
        }
        else {
            break;
        }
    }

    return postfix;
}

Starsurge::GLSL::ASTNodeExpression * Starsurge::GLSL::Parser::Unary(ASTNode * scope) {
    // UnaryPostfix
    ASTNodeExpression * postfix = UnaryPostfix(scope);
    if (postfix) {
        return postfix;
    }

    // ('+' / '-' / '!' / '~' / '++' / '--') Unary
    if (Match({TOKEN_OP_PLUS, TOKEN_OP_MINUS, TOKEN_OP_BANG, TOKEN_OP_TILDE, TOKEN_OP_INC, TOKEN_OP_DEC})) {
        Token op = Previous();
        ASTNodeExpression * unary = Unary(scope);
        if (unary) {
            ASTNodePrefixOp * ret = new ASTNodePrefixOp(op.data);
            ret->AddChild(unary);
            return ret;
        }
        else {
            Error("Invalid unary after prefix operator '"+op.data+"'");
            return NULL;
        }
    }

    return NULL;
}

Starsurge::GLSL::ASTNodeFunctionCall * Starsurge::GLSL::Parser::FunctionCall(ASTNode * scope) {
    // (Function | Type ['[' [Expression] ']']) '(' [{Expression ','} Expression] ')'
    std::string f = "";
    int num_params = -1;
    if (Type(scope)) {
        f = Previous().data;
        if (Match(TOKEN_LEFT_BRACKET)) {
            ASTNodeExpression * expr = Expression(scope);
            ASTNodeExpression * evaluated = Evaluate(expr, scope); // Get the array size.
            if (evaluated) {
                if (evaluated->GetNodeType() != AST_NODE_CONST_INT) {
                    Error("Array sizes must be integers.");
                    return NULL;
                }
                num_params = ((ASTNodeConstInt*)evaluated)->value;
                if (num_params <= 0) {
                    Error("Array sizes must be >=1.");
                    return NULL;
                }
                f = f+"["+std::to_string(num_params)+"]";
            }
            else if (!expr) {
                f = f+"[]";
            }
            else {
                Error("Arrays need to determine the array size at compile-time.");
                return NULL;
            }

            if (!Match(TOKEN_RIGHT_BRACKET)) {
                Error("Expected ']'.");
                return NULL;
            }
        }
    }
    else if (Function(scope)) {
        f = Previous().data;
    }
    else if (Check(TOKEN_IDENTIFIER)) {
        if (ElemOf<const char*>(BUILTIN_FUNCTIONS, BUILTIN_FUNCTIONS_COUNT, Peek().data.c_str())) {
            f = Peek().data;
            Advance();
        }
    }

    if (f != "") {
        if (!Match(TOKEN_LEFT_PAREN)) {
            return NULL;
        }

        unsigned int actual_num_params = 0;
        ASTNodeFunctionCall * ret = new ASTNodeFunctionCall(f);
        while (true) {
            ASTNodeExpression * parameter = Expression(scope);
            if (parameter) {
                ret->AddChild(parameter);
                actual_num_params++;
            }

            if (Match(TOKEN_RIGHT_PAREN)) {
                break;
            }
            else if (Match(TOKEN_OP_COMMA)) {
                continue;
            }
            else {
                Error("Expected either ',' or ')' in function call.");
                return NULL;
            }
        }

        if (num_params >= 0 && actual_num_params != num_params) {
            Error("Expected "+std::to_string(num_params)+" parameters but got "+std::to_string(actual_num_params)+" in function call.");
            return NULL;
        }

        // Ensure it's a valid call.
        ASTNodeStruct * strct = FindStruct(f, scope);

        std::vector<std::string> genType = {"float", "vec2", "vec3", "vec4", "color"};
        std::vector<std::string> genIType = {"int", "ivec2", "ivec3", "ivec4"};
        std::vector<std::string> genUType = {"uint", "uvec2", "uvec3", "uvec4"};
        std::vector<std::string> genBType = {"bool", "bvec2", "bvec3", "bvec4"};
        std::vector<std::string> mat = {"mat2x2", "mat2x3", "mat2x4", "mat3x2", "mat3x3", "mat3x4", "mat4x2", "mat4x3", "mat4x4",
                                        "mat2", "mat3", "mat4"};
        std::vector<std::vector<std::string>> validOptions;
        std::size_t bracket_pos = f.find('[');
        if (bracket_pos != std::string::npos) {
            std::string baseType = f.substr(0,bracket_pos);
            std::vector<std::string> params;
            for (unsigned int i = 0; i < actual_num_params; ++i) {
                params.push_back(baseType);
            }
            validOptions.push_back(params);
            f = baseType+"["+std::to_string(actual_num_params)+"]";
            ret->function = f;
        }
        else if (strct) {
            std::vector<ASTNodeField*> fields = strct->GetChildren<ASTNodeField>(AST_NODE_FIELD);

            std::vector<std::string> types;
            for (unsigned int i = 0; i < fields.size(); ++i) {
                std::string type = fields[i]->type;
                if (fields[i]->arraySize > 0) { type += "[" + std::to_string(fields[i]->arraySize) + "]"; }
                types.push_back(type);
            }
            validOptions.push_back(types);
        }
        else if (f == "bvec2" || f == "ivec2" || f == "uvec2" || f == "vec2") {
            std::string prefix = "";
            std::string type = "float";
            if (f != "vec2") { prefix += f[0]; }
            if (prefix == "b") { type = "bool"; }
            if (prefix == "i") { type = "int"; }
            if (prefix == "u") { type = "uint"; }
            validOptions.push_back(std::vector<std::string>({type}));
            validOptions.push_back(std::vector<std::string>({type,type}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec2"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec3"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec4"}));
            validOptions.push_back(std::vector<std::string>({"color"}));
        }
        else if (f == "bvec3" || f == "ivec3" || f == "uvec3" || f == "vec3") {
            std::string prefix = "";
            std::string type = "float";
            if (f != "vec3") { prefix += f[0]; }
            if (prefix == "b") { type = "bool"; }
            if (prefix == "i") { type = "int"; }
            if (prefix == "u") { type = "uint"; }
            validOptions.push_back(std::vector<std::string>({type}));
            validOptions.push_back(std::vector<std::string>({type,type,type}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec2", type}));
            validOptions.push_back(std::vector<std::string>({type, prefix+"vec2"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec3"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec4"}));
            validOptions.push_back(std::vector<std::string>({"color"}));
        }
        else if (f == "bvec4" || f == "ivec4" || f == "uvec4" || f == "vec4" || f == "color") {
            std::string prefix = "";
            std::string type = "float";
            if (f != "vec4" && f != "color") { prefix += f[0]; }
            if (prefix == "b") { type = "bool"; }
            if (prefix == "i") { type = "int"; }
            if (prefix == "u") { type = "uint"; }
            validOptions.push_back(std::vector<std::string>({type}));
            validOptions.push_back(std::vector<std::string>({type,type,type,type}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec2", type,type}));
            validOptions.push_back(std::vector<std::string>({type, prefix+"vec2", type}));
            validOptions.push_back(std::vector<std::string>({type,type, prefix+"vec2"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec3", type}));
            validOptions.push_back(std::vector<std::string>({type, prefix+"vec3"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec2", prefix+"vec2"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec4"}));
            validOptions.push_back(std::vector<std::string>({"color"}));
        }
        else if (f == "int" || f == "float" || f == "bool" || f == "uint") {
            validOptions.push_back(std::vector<std::string>({"bool"}));
            validOptions.push_back(std::vector<std::string>({"uint"}));
            validOptions.push_back(std::vector<std::string>({"int"}));
            validOptions.push_back(std::vector<std::string>({"float"}));
            std::string prefix = "";
            if (f == "bool") { prefix = "b"; }
            if (f == "uint") { prefix = "u"; }
            if (f == "int") { prefix = "i"; }
            validOptions.push_back(std::vector<std::string>({prefix+"vec2"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec3"}));
            validOptions.push_back(std::vector<std::string>({prefix+"vec4"}));
            if (prefix == "") {
                validOptions.push_back(std::vector<std::string>({"color"}));
            }
        }
        else if (ElemOf<std::string>(mat, f)) {
            unsigned int ncols = 0;
            unsigned int nrows = 0;
            if (f == "mat2") { nrows = 2; ncols = 2; }
            else if (f == "mat3") { nrows = 3; ncols = 3; }
            else if (f == "mat4") { nrows = 2; ncols = 2; }
            else {
                ncols = std::stoi(f.substr(3,1));
                nrows = std::stoi(f.substr(5,1));
            }

            std::vector<std::string> bycolumns;
            for (unsigned int i = 0; i < ncols; ++i) { bycolumns.push_back("vec"+std::to_string(nrows)); }
            validOptions.push_back(bycolumns);

            std::vector<std::string> allfloats;
            for (unsigned int i = 0; i < ncols*nrows; ++i) { allfloats.push_back("float"); }
            validOptions.push_back(allfloats);

            if (nrows > 2) {
                std::vector<std::string> partialvec2;
                for (unsigned int i = 0; i < ncols; ++i) {
                    partialvec2.push_back("vec2");
                    for (unsigned int j = 2; j < nrows; ++j) { partialvec2.push_back("float"); }
                }
                validOptions.push_back(partialvec2);
            }
            if (nrows > 3) {
                std::vector<std::string> partialvec3;
                for (unsigned int i = 0; i < ncols; ++i) {
                    partialvec3.push_back("vec3");
                    for (unsigned int j = 3; j < nrows; ++j) { partialvec3.push_back("float"); }
                }
                validOptions.push_back(partialvec3);
            }

            for (unsigned int i = ncols; i < 4; ++i) {
                for (unsigned int j = nrows; j < 4; ++j) {
                    validOptions.push_back(std::vector<std::string>({"mat"+std::to_string(i)+"x"+std::to_string(j)}));
                    if (i == j) {
                        validOptions.push_back(std::vector<std::string>({"mat"+std::to_string(i)}));
                    }
                }
            }
        }
        // GLSL Builtin functions
        else if (f == "radians" || f == "degrees" || f == "sin" || f == "asin" || f == "acos" || f == "sinh" || f == "cosh" || f == "tanh" ||
                f == "asinh" || f == "acosh" || f == "atanh" || f == "exp" || f == "log" || f == "exp2" || f == "log2" || f == "sqrt" ||
                f == "inversesqrt" || f == "floor" || f == "trunc" || f == "round" || f == "roundEven" || f == "ceil" || f == "fract" ||
                f == "isnan" || f == "isinf" || f == "floatBitsToInt" || f == "floatBitsToUint" || f == "length" || f == "normalize" ||
                f == "dFdx" || f == "dFdy" || f == "fwidth" || f == "noise1" || f == "noise2" || f == "noise3" || f == "noise4") {
            for (unsigned int i = 0; i < genType.size(); ++i) { validOptions.push_back(std::vector<std::string>({genType[i]})); }
        }
        else if (f == "EmitVertex" || f == "EndPrimitive") { // No parameters.
            std::vector<std::string> blank;
            validOptions.push_back(blank);
        }
        else if (f == "atan") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i]}));
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i]}));
            }
        }
        else if (f == "mod") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], "float"}));
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i]}));
            }
        }
        else if (f == "pow" || f == "modf" || f == "distance" || f == "dot" || f == "reflect") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i]}));
            }
        }
        else if (f == "lessThan" || f == "lessThanEqual" || f == "greaterThan" || f == "greaterThanEqual" ||
                f == "equal" || f == "notEqual") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i]}));
            }
            for (unsigned int i = 0; i < genIType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genIType[i], genIType[i]}));
            }
            for (unsigned int i = 0; i < genUType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genUType[i], genUType[i]}));
            }
        }
        else if (f == "any" || f == "all" || f == "not") {
            for (unsigned int i = 0; i < genBType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genBType[i]}));
            }
        }
        else if (f == "min" || f == "max") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i]}));
                validOptions.push_back(std::vector<std::string>({genType[i], "float"}));
            }
            for (unsigned int i = 0; i < genIType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genIType[i], genIType[i]}));
                validOptions.push_back(std::vector<std::string>({genIType[i], "int"}));
            }
            for (unsigned int i = 0; i < genUType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genUType[i], genUType[i]}));
                validOptions.push_back(std::vector<std::string>({genUType[i], "uint"}));
            }
        }
        else if (f == "clamp") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i], genType[i]}));
                validOptions.push_back(std::vector<std::string>({genType[i], "float", "float"}));
            }
            for (unsigned int i = 0; i < genIType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genIType[i], genIType[i], genIType[i]}));
                validOptions.push_back(std::vector<std::string>({genIType[i], "int", "int"}));
            }
            for (unsigned int i = 0; i < genUType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genUType[i], genUType[i], genUType[i]}));
                validOptions.push_back(std::vector<std::string>({genUType[i], "uint", "uint"}));
            }
        }
        else if (f == "mix") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i], genType[i]}));
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i], "float"}));
            }
            for (unsigned int i = 0; i < genBType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i], genBType[i]}));
            }
            validOptions.push_back(std::vector<std::string>({"color", "color", "bvec4"}));
        }
        else if (f == "step") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i]}));
                validOptions.push_back(std::vector<std::string>({"float", genType[i]}));
            }
        }
        else if (f == "smoothstep") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i], genType[i]}));
                validOptions.push_back(std::vector<std::string>({"float", "float", genType[i]}));
            }
        }
        else if (f == "intBitsToFloat") {
            for (unsigned int i = 0; i < genIType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genIType[i]}));
            }
        }
        else if (f == "uintBitsToFloat") {
            for (unsigned int i = 0; i < genUType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genUType[i]}));
            }
        }
        else if (f == "cross") {
            validOptions.push_back(std::vector<std::string>({"vec3", "vec3"}));
        }
        else if (f == "faceforward") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i], genType[i]}));
            }
        }
        else if (f == "refract") {
            for (unsigned int i = 0; i < genType.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({genType[i], genType[i], "float"}));
            }
        }
        else if (f == "matrixCompMult") {
            for (unsigned int i = 0; i < mat.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({mat[i], mat[i]}));
            }
        }
        else if (f == "outerProduct") {
            for (unsigned int i = 1; i < genType.size(); ++i) {
                for (unsigned int j = 1; j < genType.size(); ++j) {
                    validOptions.push_back(std::vector<std::string>({genType[i], genType[j]}));
                }
            }
        }
        else if (f == "transpose") {
            for (unsigned int i = 0; i < mat.size(); ++i) {
                validOptions.push_back(std::vector<std::string>({mat[i]}));
            }
        }
        else if (f == "determinant" || f == "inverse") {
            validOptions.push_back(std::vector<std::string>({"mat2"}));
            validOptions.push_back(std::vector<std::string>({"mat3"}));
            validOptions.push_back(std::vector<std::string>({"mat4"}));
            validOptions.push_back(std::vector<std::string>({"mat2x2"}));
            validOptions.push_back(std::vector<std::string>({"mat3x3"}));
            validOptions.push_back(std::vector<std::string>({"mat4x4"}));
        }
        else if (f == "textureSize") {
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler3D", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsamplerCube", "int"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DShadow", "int"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DShadow", "int"}));
            validOptions.push_back(std::vector<std::string>({"samplerCubeShadow", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DRect", "int"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DRectShadow", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler1DArray", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DArray", "int"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DArrayShadow", "int"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DArrayShadow", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsamplerBuffer", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DMS", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DMSArray", "int"}));
        }
        else if (f == "texture" || f == "textureOffset") {
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"gsampler3D", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DShadow", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DShadow", "vec4"}));
            validOptions.push_back(std::vector<std::string>({"gsampler1DArray", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DArray", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DArrayShadow", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DArrayShadow", "vec4"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DRect", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DRectShadow", "vec3"}));
            if (f == "texture") {
                validOptions.push_back(std::vector<std::string>({"gsamplerCube", "vec3"}));
                validOptions.push_back(std::vector<std::string>({"samplerCubeShadow", "vec4"}));
                validOptions.push_back(std::vector<std::string>({"gsamplerCubeArray", "vec4"}));
                validOptions.push_back(std::vector<std::string>({"samplerCubeArrayShadow", "vec4"}));
            }

            if (f == "textureOffset") {
                for (unsigned int i = 0; i < validOptions.size(); ++i) {
                    if (validOptions[i][0] == "gsampler1D" || validOptions[i][0] == "sampler1DShadow" ||
                        validOptions[i][0] == "gsampler1DArray") { validOptions[i].push_back("int"); }
                    else if (validOptions[i][0] == "gsampler3D" || validOptions[i][0] == "gsampler2DArray") { validOptions[i].push_back("ivec3"); }\
                    else { validOptions[i].push_back("ivec2"); }
                }
            }

            // Add optional bias.
            unsigned int presize = validOptions.size();
            for (unsigned int i = 0; i < presize; ++i) {
                if (validOptions[i][0] != "gsampler2DRect" && validOptions[i][0] != "sampler2DShadow" && validOptions[i][0] == "sampler1DArrayShadow") {
                    std::vector<std::string> copy = validOptions[i];
                    copy.push_back("float");
                    validOptions.push_back(copy);
                }
            }
        }
        else if (f == "texelFetch" || f == "texelFetchOffset") {
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "int", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "ivec2", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler3D", "ivec3", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler1DArray", "ivec2", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DArray", "ivec3", "int"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DRect", "ivec2"}));
            if (f == "texelFetch") {
                validOptions.push_back(std::vector<std::string>({"gsamplerBuffer", "int"}));
                validOptions.push_back(std::vector<std::string>({"gsampler2DMS", "ivec2", "int"}));
                validOptions.push_back(std::vector<std::string>({"gsampler2DMSArray", "ivec3", "int"}));
            }

            if (f == "texelFetchOffset") {
                for (unsigned int i = 0; i < validOptions.size(); ++i) {
                    if (validOptions[i][0] == "gsampler1D") { validOptions[i].push_back("int"); }
                    else if (validOptions[i][0] == "gsampler3D" || validOptions[i][0] == "gsampler2DArray") { validOptions[i].push_back("ivec3"); }
                    else { validOptions[i].push_back("ivec2"); }
                }
            }
        }
        else if (f == "textureProj" || f == "textureProjOffset") {
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "vec4"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec4"}));
            validOptions.push_back(std::vector<std::string>({"gsampler3D", "vec4"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DShadow", "vec4"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DShadow", "vec4"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DRect", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DRect", "vec4"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DRectShadow", "vec4"}));

            if (f == "textureProjOffset") {
                for (unsigned int i = 0; i < validOptions.size(); ++i) {
                    if (validOptions[i][0] == "gsampler1D" || validOptions[i][0] == "sampler1DShadow") { validOptions[i].push_back("int"); }
                    else if (validOptions[i][0] == "gsampler3D") { validOptions[i].push_back("ivec3"); }
                    else { validOptions[i].push_back("ivec2"); }
                }
            }

            // Add optional bias.
            unsigned int presize = validOptions.size();
            for (unsigned int i = 0; i < presize; ++i) {
                if (validOptions[i][0] != "sampler2DRect" && validOptions[i][0] == "sampler2DRectShadow") {
                    std::vector<std::string> copy = validOptions[i];
                    copy.push_back("float");
                    validOptions.push_back(copy);
                }
            }
        }
        else if (f == "textureLod" || f == "textureLodOffset") {
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "float", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec2", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler3D", "vec3", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsamplerCube", "vec3", "float"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DShadow", "vec3", "float"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DShadow", "vec3", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler1DArray", "vec2", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DArray", "vec2", "float"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DArrayShadow", "vec2", "float"}));

            if (f == "textureLodOffset") {
                for (unsigned int i = 0; i < validOptions.size(); ++i) {
                    if (validOptions[i][0] == "gsampler1D" || validOptions[i][0] == "sampler1DShadow" ||
                        validOptions[i][0] == "gsampler1DArray" || validOptions[i][0] == "sampler1DArrayShadow") { validOptions[i].push_back("int"); }
                    else if (validOptions[i][0] == "gsampler3D") { validOptions[i].push_back("ivec3"); }
                    else { validOptions[i].push_back("ivec2"); }
                }
            }
        }
        else if (f == "textureGrad" || f == "textureGradOffset") {
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "float", "float", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec2", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"gsampler3D", "vec3", "vec3", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"gsamplerCube", "vec3", "vec3", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DShadow", "vec3", "float", "float"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DShadow", "vec3", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"samplerCubeShadow", "vec4", "vec3", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"gsampler1DArray", "vec2", "float", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DArray", "vec3", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DArrayShadow", "vec3", "float", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsamplerCubeArray", "vec4", "vec3", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DRect", "vec2", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DRectShadow", "vec2", "vec2", "vec2"}));

            if (f == "textureGradOffset") {
                for (unsigned int i = 0; i < validOptions.size(); ++i) {
                    if (validOptions[i][0] == "gsampler1D" || validOptions[i][0] == "sampler1DShadow" ||
                        validOptions[i][0] == "gsampler1DArray" || validOptions[i][0] == "sampler1DArrayShadow") { validOptions[i].push_back("int"); }
                    else if (validOptions[i][0] == "gsampler3D") { validOptions[i].push_back("ivec3"); }
                    else { validOptions[i].push_back("ivec2"); }
                }
                validOptions.push_back(std::vector<std::string>({"sampler2DArrayShadow", "vec3", "vec2", "vec2", "ivec2"}));
            }
        }
        else if (f == "textureProjLod" || f == "textureProjLodOffset") {
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "vec2", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "vec4", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec3", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec4", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler3D", "vec4", "float"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DShadow", "vec4", "float"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DShadow", "vec4", "float"}));

            if (f == "textureProjLodOffset") {
                for (unsigned int i = 0; i < validOptions.size(); ++i) {
                    if (validOptions[i][0] == "gsampler1D" || validOptions[i][0] == "sampler1DShadow") { validOptions[i].push_back("int"); }
                    else if (validOptions[i][0] == "gsampler3D") { validOptions[i].push_back("ivec3"); }
                    else { validOptions[i].push_back("ivec2"); }
                }
            }
        }
        else if (f == "textureProjGrad" || f == "textureProjGradOffset") {
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "vec2", "float", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler1D", "vec4", "float", "float"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec3", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2D", "vec4", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"gsampler3D", "vec4", "vec3", "vec3"}));
            validOptions.push_back(std::vector<std::string>({"sampler1DShadow", "vec4", "float", "float"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DShadow", "vec4", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DRect", "vec3", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"gsampler2DRect", "vec4", "vec2", "vec2"}));
            validOptions.push_back(std::vector<std::string>({"sampler2DRectShadow", "vec4", "vec2", "vec2"}));

            if (f == "textureProjGradOffset") {
                for (unsigned int i = 0; i < validOptions.size(); ++i) {
                    if (validOptions[i][0] == "gsampler1D" || validOptions[i][0] == "sampler1DShadow") { validOptions[i].push_back("int"); }
                    else if (validOptions[i][0] == "gsampler3D") { validOptions[i].push_back("ivec3"); }
                    else { validOptions[i].push_back("ivec2"); }
                }
            }
        }
        else {
            std::vector<ASTNodeFunction*> matches = FindFunction(f, scope);
            if (matches.size() > 0) {
                for (unsigned int i = 0; i < matches.size(); ++i) {
                    std::vector<std::string> option;
                    for (unsigned int j = 0; j < matches[i]->parameters.size(); ++j) {
                        option.push_back(matches[i]->parameters[j].type);
                    }
                    validOptions.push_back(option);
                }
            }
        }

        std::vector<std::string> params;
        for (unsigned int i = 0; i < actual_num_params; ++i) {
            params.push_back(EvaluateType((ASTNodeExpression*)ret->GetChild(i), scope));
        }

        for (unsigned int i = 0; i < validOptions.size(); ++i) {
            if (validOptions[i].size() == 0 && actual_num_params == 0) {
                return ret;
            }
            if (validOptions[i].size() == actual_num_params) {
                bool isMatch = true;
                for (unsigned int j = 0; j < actual_num_params; ++j) {
                    if (CompatibleTypes(validOptions[i][j], params[j])) {
                        continue;
                    }
                    else {
                        isMatch = false;
                        break;
                    }
                }

                if (isMatch) {
                    return ret;
                }
            }
        }

        std::string call = f;
        call += "(";
        for (unsigned int j = 0; j < actual_num_params; ++j) {
            if (j > 0) call += ", ";
            call += params[j];
        }
        call += ")";
        Error("No matching definition for function call '"+call+"'.");
        return NULL;
    }

    return NULL;
}

int Starsurge::GLSL::Parser::ArrayDeclare(ASTNode * scope) {
    if (Match(TOKEN_LEFT_BRACKET)) {
        ASTNodeExpression * expr = Expression(scope);
        if (!expr) {
            Error("Expected expression between '[' and ']'.");
            return -1;
        }
        ASTNodeExpression * evaluated = Evaluate(expr, scope);
        if (evaluated) {
            if (evaluated->GetNodeType() != AST_NODE_CONST_INT) {
                Error("Array sizes should be given by positive integers.");
                return -1;
            }
            ASTNodeConstInt * cast = (ASTNodeConstInt*)evaluated;
            if (cast->value <= 0) {
                Error("Array size should be >=1.");
                return -1;
            }
            if (Match(TOKEN_RIGHT_BRACKET)) {
                if (Match(TOKEN_LEFT_BRACKET)) {
                    Error("Multidimensional arrays are currently unsupported.");
                    return -1;
                }
                return cast->value;
            }
            else {
                Error("Expected ']'.");
                return -1;
            }
        }
        else {
            Error("Arrays need to determine the array size at compile-time.");
            return -1;
        }
    }

    return 0;
}

bool Starsurge::GLSL::Parser::Operator() {
    return OperatorArithmetic() || OperatorTest() || OperatorAssign() || OperatorBitwise();
}

bool Starsurge::GLSL::Parser::OperatorArithmetic() {
    return Check({TOKEN_OP_STAR, TOKEN_OP_SLASH, TOKEN_OP_PLUS, TOKEN_OP_MINUS, TOKEN_OP_MOD});
}

bool Starsurge::GLSL::Parser::OperatorTest() {
    return Check({TOKEN_OP_LT, TOKEN_OP_GT, TOKEN_OP_LTEQ, TOKEN_OP_GTEQ, TOKEN_OP_EQUALS, TOKEN_OP_NOT_EQUALS,
        TOKEN_OP_AND, TOKEN_OP_XOR, TOKEN_OP_OR});
}

bool Starsurge::GLSL::Parser::OperatorAssign() {
    return Check({TOKEN_OP_PLUS_EQUAL, TOKEN_OP_MINUS_EQUAL, TOKEN_OP_STAR_EQUAL, TOKEN_OP_SLASH_EQUAL, TOKEN_OP_MOD_EQUAL, TOKEN_OP_LEFT_EQUAL,
        TOKEN_OP_RIGHT_EQUAL, TOKEN_OP_BITWISE_AND_EQUAL, TOKEN_OP_BITWISE_XOR_EQUAL, TOKEN_OP_BITWISE_OR_EQUAL, TOKEN_OP_ASSIGN});
}

bool Starsurge::GLSL::Parser::OperatorBitwise() {
    return Check({TOKEN_OP_LEFT, TOKEN_OP_RIGHT, TOKEN_OP_BITWISE_AND, TOKEN_OP_BITWISE_XOR, TOKEN_OP_BITWISE_OR});
}

bool Starsurge::GLSL::Parser::Type(ASTNode * scope) {
    if (Match(TOKEN_TYPE)) {
        return true;
    }

    // Search through the tree for scopes with the right name.
    if (Check(TOKEN_IDENTIFIER)) {
        std::string search = Peek().data;
        // Check builtin first.
        // if (Starsurge::ElemOf<const char*>(BUILTIN_STRUCTS, BUILTIN_STRUCTS_COUNT, search.c_str())) {
        //     Advance();
        //     return true;
        // }

        ASTNodeStruct * strct = FindStruct(search, scope);
        if (strct) {
            Advance();
            return true;
        }
    }

    return false;
}

bool Starsurge::GLSL::Parser::Variable(ASTNode * scope) {
    // Checks to the see i the next token is valid variable name.
    if (Check(TOKEN_IDENTIFIER)) {
        std::string search = Peek().data;
        ASTNodeVariableDeclaration * var = FindVariable(search, scope);
        if (var) {
            Advance();
            return true;
        }
    }

    return false;
}

bool Starsurge::GLSL::Parser::Function(ASTNode * scope) {
    if (Check(TOKEN_IDENTIFIER)) {
        std::string search = Peek().data;
        // Check builtin first.
        if (Starsurge::ElemOf<const char*>(BUILTIN_FUNCTIONS, BUILTIN_FUNCTIONS_COUNT, search.c_str())) {
            Advance();
            return true;
        }

        ASTNode * curScope = scope->GetScope();
        while (curScope != NULL) {
            std::vector<ASTNodeFunction*> functions = curScope->GetChildren<ASTNodeFunction>(AST_NODE_FUNCTION);
            for (unsigned int i = 0; i < functions.size(); ++i) {
                if (functions[i]->name == search) {
                    Advance();
                    return true;
                }
            }

            curScope = curScope->GetHigherScope();
        }
    }

    return false;
}


Starsurge::GLSL::ASTNodeStruct * Starsurge::GLSL::Parser::FindStruct(std::string search, ASTNode * scope) {
    ASTNode * curScope = scope->GetScope();
    while (curScope != NULL) {
        std::vector<ASTNodeStruct*> structs = curScope->GetChildren<ASTNodeStruct>(AST_NODE_STRUCT);
        for (unsigned int i = 0; i < structs.size(); ++i) {
            if (structs[i]->name == search) {
                return structs[i];
            }
        }

        curScope = curScope->GetHigherScope();
    }

    return NULL;
}

Starsurge::GLSL::ASTNodeVariableDeclaration * Starsurge::GLSL::Parser::FindVariable(std::string search, ASTNode * scope) {
    ASTNode * curScope = scope->GetScope();
    while (curScope != NULL) {
        std::vector<ASTNodeVariableDeclaration*> variables = curScope->GetChildren<ASTNodeVariableDeclaration>(AST_NODE_VARIABLE_DECLARATION);
        for (unsigned int i = 0; i < variables.size(); ++i) {
            if (variables[i]->name == search) {
                return variables[i];
            }
        }

        // Check the intializer portion of for loops
        if (curScope->GetParent() != NULL && curScope->GetParent()->GetNodeType() == AST_NODE_FOR) {
            ASTNodeFor * node = (ASTNodeFor*)curScope->GetParent();
            ASTNode * declares = node->GetDeclares();
            if (declares && declares->GetNodeType() == AST_NODE_VARIABLE_DECLARATION) {
                ASTNodeVariableDeclaration * cast = (ASTNodeVariableDeclaration*)declares;
                if (cast->name == search) {
                    return cast;
                }
            }
            else if (declares && declares->ChildCount() > 1) {
                std::vector<ASTNodeVariableDeclaration*> variables = curScope->GetChildren<ASTNodeVariableDeclaration>(AST_NODE_VARIABLE_DECLARATION);
                for (unsigned int i = 0; i < variables.size(); ++i) {
                    if (variables[i]->name == search) {
                        return variables[i];
                    }
                }
            }
        }

        // Check the parameter portion of functions
        if (curScope->GetNodeType() == AST_NODE_FUNCTION) {
            ASTNodeFunction * functionHeader = (ASTNodeFunction*)curScope;

            for (unsigned int i = 0; i < functionHeader->parameters.size(); ++i) {
                if (functionHeader->parameters[i].name == search) {
                    ASTNodeVariableDeclaration * declare = new ASTNodeVariableDeclaration();
                    declare->name = functionHeader->parameters[i].name;
                    declare->type = functionHeader->parameters[i].type;
                    declare->arraySize = functionHeader->parameters[i].arraySize;
                    return declare;
                }
            }
        }

        curScope = curScope->GetHigherScope();
    }
    return NULL;
}

std::vector<Starsurge::GLSL::ASTNodeFunction*> Starsurge::GLSL::Parser::FindFunction(std::string search, ASTNode * scope) {
    std::vector<ASTNodeFunction*> matches;
    ASTNode * curScope = scope->GetScope();
    while (curScope != NULL) {
        std::vector<ASTNodeFunction*> functions = curScope->GetChildren<ASTNodeFunction>(AST_NODE_FUNCTION);
        for (unsigned int i = 0; i < functions.size(); ++i) {
            if (functions[i]->name == search) {
                matches.push_back(functions[i]);
            }
        }

        curScope = curScope->GetHigherScope();
    }
    return matches;
}

bool Starsurge::GLSL::Parser::CompatibleTypes(std::string in_type1, std::string in_type2) {
    std::string type1 = in_type1;
    std::string type2 = in_type2;
    for (unsigned int i = 0; i < 2; ++i) {
        if (type1 == type2) {
            return true;
        }
        else if ((type1 == "float" || type1 == "int" || type1 == "uint" || type1 == "bool") &&
            (type2 == "float" || type2 == "int" || type2 == "uint" || type2 == "bool")) {
            return true;
        }
        else if (type1 == "gsampler1D" && (type2 == "sampler1D" || type2 == "isampler1D" || type2 == "usampler1D")) {
            return true;
        }
        else if (type1 == "gsampler2D" && (type2 == "sampler2D" || type2 == "isampler2D" || type2 == "usampler2D")) {
            return true;
        }
        else if (type1 == "gsampler3D" && (type2 == "sampler3D" || type2 == "isampler3D" || type2 == "usampler3D")) {
            return true;
        }
        else if (type1 == "gsamplerCube" && (type2 == "samplerCube" || type2 == "isamplerCube" || type2 == "usamplerCube")) {
            return true;
        }
        else if (type1 == "gsampler2DRect" && (type2 == "sampler2DRect" || type2 == "isampler2DRect" || type2 == "usampler2DRect")) {
            return true;
        }
        else if (type1 == "gsampler1DArray" && (type2 == "sampler1DArray" || type2 == "isampler1DArray" || type2 == "usampler1DArray")) {
            return true;
        }
        else if (type1 == "gsampler2DArray" && (type2 == "sampler2DArray" || type2 == "isampler2DArray" || type2 == "usampler2DArray")) {
            return true;
        }
        else if (type1 == "gsamplerBuffer" && (type2 == "samplerBuffer" || type2 == "isamplerBuffer" || type2 == "usamplerBuffer")) {
            return true;
        }
        else if (type1 == "gsampler2DMS" && (type2 == "sampler2DMS" || type2 == "isampler2DMS" || type2 == "usampler2DMS")) {
            return true;
        }
        else if (type1 == "gsampler2DMSArray" && (type2 == "sampler2DMSArray" || type2 == "isampler2DMSArray" || type2 == "usampler2DMSArray")) {
            return true;
        }
        else if (type1 == "gvec4" && (type2 == "vec4" || type2 == "color" || type2 == "ivec4" || type2 == "uvec4")) {
            return true;
        }
        else if (type1 == "vec4" && type2 == "color") {
            return true;
        }
        else if (type1 == "mat2" && type2 == "mat2x2") {
            return true;
        }
        else if (type1 == "mat3" && type2 == "mat3x3") {
            return true;
        }
        else if (type1 == "mat4" && type2 == "mat4x4") {
            return true;
        }

        type1 = in_type2;
        type2 = in_type1;
    }
    return false;
}
