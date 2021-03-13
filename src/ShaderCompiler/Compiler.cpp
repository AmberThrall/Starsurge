#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../include/ShaderCompiler/Compiler.h"
#include "../../include/Utils.h"
#include "../../include/Logging.h"
#include "../../include/Shader.h"

Starsurge::GLSL::Compiler::Compiler(std::string source_code) {
    SetCode(source_code);
}

void Starsurge::GLSL::Compiler::SetCode(std::string source_code) {
    code = source_code;
}

bool Starsurge::GLSL::Compiler::Compile(Shader * shader) {
    CodeStructure structure = ParseStructure();
    if (errors.size() > 0) {
        for (unsigned int i = 0; i < errors.size(); ++i) {
            ShaderError(errors[i]);
        }
        return false;
    }

    if (structure.shaderType == "Shader") {
        for (unsigned int i = 0; i < structure.passes.size(); ++i) {
            ShaderPass shaderPass;
            shaderPass.vertex = CompileVertex(structure.uniforms, structure.codeOffset[i], structure.passes[i]);
            if (errors.size() > 0) {
                for (unsigned int i = 0; i < errors.size(); ++i) {
                    ShaderError(errors[i]);
                }
                shader->passes.clear();
                return false;
            }
            shaderPass.fragment = CompileFragment(structure.uniforms, structure.codeOffset[i], structure.passes[i]);
            if (errors.size() > 0) {
                for (unsigned int i = 0; i < errors.size(); ++i) {
                    ShaderError(errors[i]);
                }
                shader->passes.clear();
                return false;
            }

            int success;
            char infoLog[512];
            shaderPass.program = glCreateProgram();
            glAttachShader(shaderPass.program, shaderPass.vertex);
            glAttachShader(shaderPass.program, shaderPass.fragment);
            glLinkProgram(shaderPass.program);
            glGetProgramiv(shaderPass.program, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shaderPass.program, 512, NULL, infoLog);
                ShaderError(infoLog);
                return false;
            }
            shader->passes.push_back(shaderPass);
        }

        // Parse the uniforms.
        GLSL::Parser parser(structure.uniforms);
        parser.Parse();
        std::vector<ASTNodeVariableDeclaration*> vars = parser.GetAST().root->GetChildren<ASTNodeVariableDeclaration>(AST_NODE_VARIABLE_DECLARATION);
        for (unsigned int i = 0; i < vars.size(); ++i) {
            shader->uniforms.push_back(ParseUniform(vars[i]));
        }
        return true;
    }
    else {
        ShaderError("Unknown shader type: "+structure.shaderType);
    }
}

unsigned int Starsurge::GLSL::Compiler::CompileVertex(std::string uniforms, unsigned int linenoOffset, std::string vert_code) {
    unsigned int ret = 0;
    int success;
    char infoLog[512];

    GLSL::Parser parser(uniforms);
    parser.Parse();
    std::vector<ASTNodeVariableDeclaration*> vars = parser.GetAST().root->GetChildren<ASTNodeVariableDeclaration>(AST_NODE_VARIABLE_DECLARATION);
    uniforms = "";
    for (unsigned int i = 0; i < vars.size(); ++i) {
        vars[i]->storage = "uniform";
        uniforms += Node2Code(vars[i]) + ";\n";
    }

    vert_code = uniforms + R"(
        struct VertexData {
            vec3 Position;
            vec3 Normal;
            vec2 UV;
            color Color;
        };
    )" + "\n" + vert_code;

    parser.SetCode(vert_code);
    parser.Parse();
    std::vector<GLSL::ParseError> errors = parser.GetErrors();
    for (unsigned int i = 0; i < errors.size(); ++i) {
        Error(errors[i].lineno+linenoOffset, errors[i].msg);
    }
    if (errors.size() > 0) { return ret; }

    vert_code = TranslateCode(parser.GetAST().root, "vertex");
    vert_code = R"(
        #version 330 core
        layout (location = 0) in vec3 _internal_Position;
        layout (location = 1) in vec3 _internal_Normal;
        layout (location = 2) in vec2 _internal_UV;
        layout (location = 3) in vec4 _internal_Color;
    )"+std::string("\n")+vert_code + R"(
        void main() {
            VertexData vertexData;
            vertexData.Position = _internal_Position;
            vertexData.Normal = _internal_Normal;
            vertexData.UV = _internal_UV;
            vertexData.Color = _internal_Color;
            gl_Position = vertex(vertexData);
        }
    )";
    const char * vert_code_c_str = vert_code.c_str();

    ret = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(ret, 1, &vert_code_c_str, NULL);
    glCompileShader(ret);
    glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ret, 512, NULL, infoLog);
        HandleGLSLErrors(linenoOffset-11-vars.size(), std::string(infoLog));
    }
    return ret;
}

unsigned int Starsurge::GLSL::Compiler::CompileFragment(std::string uniforms, unsigned int linenoOffset, std::string frag_code) {
    unsigned int ret = 0;
    int success;
    char infoLog[512];

    GLSL::Parser parser(uniforms);
    parser.Parse();
    std::vector<ASTNodeVariableDeclaration*> vars = parser.GetAST().root->GetChildren<ASTNodeVariableDeclaration>(AST_NODE_VARIABLE_DECLARATION);
    uniforms = "";
    for (unsigned int i = 0; i < vars.size(); ++i) {
        vars[i]->storage = "uniform";
        uniforms += Node2Code(vars[i]) + ";\n";
    }

    frag_code = uniforms + R"(
        struct VertexData {
            vec3 Position;
            vec3 Normal;
            vec2 UV;
            color Color;
        };
    )" + "\n" + frag_code;

    parser.SetCode(frag_code);
    parser.Parse();
    std::vector<GLSL::ParseError> errors = parser.GetErrors();
    for (unsigned int i = 0; i < errors.size(); ++i) {
        Error(errors[i].lineno+linenoOffset, errors[i].msg);
    }
    if (errors.size() > 0) { return ret; }

    frag_code = TranslateCode(parser.GetAST().root, "fragment");
    frag_code = "#version 330 core\nout vec4 _internal_FragColor;\n\n"+frag_code;
    frag_code += R"(
        void main() {
            _internal_FragColor = fragment();
        }
    )";
    const char * frag_code_c_str = frag_code.c_str();

    ret = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(ret, 1, &frag_code_c_str, NULL);
    glCompileShader(ret);
    glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ret, 512, NULL, infoLog);
        HandleGLSLErrors(linenoOffset-1-vars.size(), std::string(infoLog));
    }
    return ret;
}

void Starsurge::GLSL::Compiler::HandleGLSLErrors(int linenoOffset, std::string log) {
    std::vector<std::string> errors = Explode(log, '\n', false);
    for (unsigned int i = 0; i < errors.size(); ++i) {
        std::size_t lparen = errors[i].find("(");
        std::size_t rparen = errors[i].find(")");
        std::size_t colon = errors[i].find(":");

        if (lparen != std::string::npos && rparen != std::string::npos && colon != std::string::npos) {
            std::string msg = errors[i].substr(colon+2);
            int lineno = std::stoi(errors[i].substr(lparen+1, rparen-lparen-1));
            std::vector<Match> matches = FindSubstrings(msg, "0\\([[:digit:]]+\\)");
            std::string newMsg = "";
            unsigned int last_pos = 0;
            for (unsigned int i = 0; i < matches.size(); ++i) {
                int lineno = std::stoi(FindFirstSubstrings(matches[i].matches[0], "\\([[:digit:]]+").matches[0].substr(1));
                newMsg += msg.substr(last_pos, matches[i].position);
                newMsg += "(" + std::to_string(lineno+linenoOffset) + ")";
                last_pos = matches[i].position+matches[i].matches[0].length();
            }
            newMsg += msg.substr(last_pos);
            Error(lineno+linenoOffset, newMsg);
        }
        else { Error(linenoOffset, errors[i]); }
    }
}

std::string Starsurge::GLSL::Compiler::TranslateCode(ASTNode * root, std::string entry) {
    if (!root) { return ""; }

    std::vector<std::string> deps;
    deps.push_back(entry);
    unsigned int oldDepsSize = 0;

    std::vector<ASTNode*> statements = root->GetAllChildren();
    while (oldDepsSize != deps.size()) { // If we add a new dependency, rescan.
        oldDepsSize = deps.size();
        for (unsigned int i = 0; i < statements.size(); ++i) {
            if (statements[i]->GetNodeType() == AST_NODE_FUNCTION) {
                ASTNodeFunction * cast = (ASTNodeFunction*)statements[i];
                if (ElemOf<std::string>(deps, cast->name)) {
                    BuildDeps(statements[i], deps);
                }
            }
            else {
                BuildDeps(statements[i], deps);
            }
        }
    }

    std::string ret = "";
    for (unsigned int i = 0; i < statements.size(); ++i) {
        if (statements[i]->GetNodeType() == AST_NODE_VARIABLE_DECLARATION) {
            ASTNodeVariableDeclaration * cast = (ASTNodeVariableDeclaration*)statements[i];
            if (cast->storage == "varying" && entry == "vertex") { cast->storage = "out"; }
            else if (cast->storage == "varying" && entry == "fragment") { cast->storage = "in"; }
        }

        if (statements[i]->GetNodeType() == AST_NODE_FUNCTION) {
            ASTNodeFunction * cast = (ASTNodeFunction*)statements[i];
            if (!ElemOf<std::string>(deps, cast->name)) {
                continue;
            }
            ret += Node2Code(statements[i]);
            ret += "\n";
        }
        else {
            ret += Node2Code(statements[i])+";";
            ret += "\n";
        }
    }

    return ret;
}

void Starsurge::GLSL::Compiler::Error(unsigned int lineno, std::string msg) {
    errors.push_back("Line "+std::to_string(lineno)+": "+msg);
}

Starsurge::GLSL::CodeStructure Starsurge::GLSL::Compiler::ParseStructure() {
    CodeStructure ret;
    ret.shaderType = "";

    unsigned int lineno = 1;
    unsigned int index = 0;
    for (; index < code.length(); ++index) {
        char c = code[index];
        if (c == '\n') { lineno++; }
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n' || c == '\f' || c == '\v') { continue; }

        if (c == '{') {
            index += 1;
            break;
        }
        else {
            ret.shaderType += c;
        }
    }

    std::string word = "";
    for (; index < code.length(); ++index) {
        char c = code[index];
        if (c == '\n') { lineno++; }
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n' || c == '\f' || c == '\v') { continue; }

        if (c == '{') {
            index += 1;
            if (word != "Pass" && word != "Uniforms") {
                Error(lineno, "Unknown keyword: "+word);
                ret.passes.clear();
                ret.uniforms = "";
                return ret;
            }

            // Read the code in it's entirety.
            ret.codeOffset.push_back(lineno+1);
            int scope = 1;
            std::string pass = "";
            bool inBlockComment = false;
            for (; index < code.length(); ++index) {
                char c = code[index];
                if (c == '\n') { lineno++; }

                if (inBlockComment) {
                    pass += c;
                    if (c == '*' && code[index+1] == '/') {
                        pass += "/";
                        inBlockComment = false;
                    }
                }
                else if (c == '/') {
                    if (code[index+1] == '/') {
                        pass += "//";
                        index++;
                        while (code[index] != '\n') { index += 1; pass += code[index]; }
                    }
                    else if (code[index+1] == '*') {
                        index++;
                        pass += "/*";
                        inBlockComment = true;
                    }
                    else {
                        pass += c;
                    }
                }
                else if (c == '{') {
                    scope += 1;
                    pass += c;
                }
                else if (c == '}') {
                    scope -= 1;
                    if (scope == 0) {
                        break;
                    }
                    pass += c;
                }
                else {
                    pass += c;
                }
            }
            if (scope != 0) {
                Error(lineno, "Missing '}' at end of "+word+".");
                ret.passes.clear();
                ret.uniforms = "";
                return ret;
            }

            if (word == "Pass") {
                ret.passes.push_back(pass);
            }
            else if (word == "Uniforms") {
                if (ret.uniforms != "") {
                    Error(lineno, "Multiple 'Uniform' declarations encounted.");
                    ret.passes.clear();
                    ret.uniforms = "";
                    return ret;
                }
                ret.uniforms = pass;
            }

            word = "";
        }
        else if (c == '}') {
            break;
        }
        else {
            word += c;
        }
    }

    if (code[index] != '}') {
        Error(lineno, "Missing '}' at end of pass.");
        ret.passes.clear();
    }

    return ret;
}

Starsurge::Uniform Starsurge::GLSL::Compiler::ParseUniform(ASTNodeVariableDeclaration * var) {
    Uniform ret(var->name, var->type);
    // TODO: Default values.
    return ret;
}

void Starsurge::GLSL::Compiler::BuildDeps(ASTNode * entry, std::vector<std::string> & deps) {
    if (!entry) { return; }
    else if (entry->GetNodeType() == AST_NODE_FUNCTION_CALL) {
        ASTNodeFunctionCall * cast = (ASTNodeFunctionCall*)entry;
        if (!ElemOf<std::string>(deps, cast->function)) {
            deps.push_back(cast->function);
        }
    }
    std::vector<ASTNode*> children = entry->GetAllChildren();
    for (unsigned int i = 0; i < children.size(); ++i) {
        BuildDeps(children[i], deps);
    }
}

std::string Starsurge::GLSL::Compiler::Node2Code(ASTNode * node) {
    if (!node) { return ""; }
    else if (node->GetNodeType() == AST_NODE_CONST_BOOL) {
        return ((ASTNodeConstBool*)node)->value ? "true" : "false";
    }
    else if (node->GetNodeType() == AST_NODE_CONST_INT) {
        return std::to_string(((ASTNodeConstInt*)node)->value);
    }
    else if (node->GetNodeType() == AST_NODE_CONST_FLOAT) {
        return std::to_string(((ASTNodeConstFloat*)node)->value)+"f";
    }
    else if (node->GetNodeType() == AST_NODE_IDENTIFIER) {
        return ((ASTNodeIdentifier*)node)->id;
    }
    else if (node->GetNodeType() == AST_NODE_VARIABLE) {
        return ((ASTNodeVariable*)node)->name;
    }
    else if (node->GetNodeType() == AST_NODE_EXPRESSION_STATEMENT) {
        return Node2Code(node->GetChild(0));
    }
    else if (node->GetNodeType() == AST_NODE_PAREN_EXPRESSION) {
        return "("+Node2Code(node->GetChild(0))+")";
    }
    else if (node->GetNodeType() == AST_NODE_PREFIX_OP) {
        ASTNodePrefixOp * cast = (ASTNodePrefixOp*)node;
        std::string rhs = Node2Code(cast->GetChild(0));
        return cast->op + rhs;
    }
    else if (node->GetNodeType() == AST_NODE_JUMP) {
        ASTNodeJump * cast = (ASTNodeJump*)node;
        if (cast->keyword == "return" && cast->ChildCount() == 1) {
            return "return "+Node2Code(cast->GetChild(0));
        }
        return cast->keyword;
    }
    else if (node->GetNodeType() == AST_NODE_POSTFIX_OP) {
        ASTNodePostfixOp * cast = (ASTNodePostfixOp*)node;
        std::string lhs = Node2Code(cast->GetChild(0));
        if (cast->op == "[]") {
            std::string arraySize = Node2Code(cast->GetChild(1));
            return lhs+"["+arraySize+"]";
        }
        return lhs + cast->op;
    }
    else if (node->GetNodeType() == AST_NODE_BINARY) {
        ASTNodeBinary * cast = (ASTNodeBinary*)node;
        std::string lhs = Node2Code(cast->GetLeft());
        std::string rhs = Node2Code(cast->GetRight());
        return lhs + cast->op + rhs;
    }
    else if (node->GetNodeType() == AST_NODE_TERNARY) {
        ASTNodeTernary * cast = (ASTNodeTernary*)node;
        std::string condition = Node2Code(cast->GetCondition());
        std::string trueResult = Node2Code(cast->GetTrue());
        std::string falseResult = Node2Code(cast->GetFalse());
        return condition + " ? "+trueResult + " : "+falseResult;
    }
    else if (node->GetNodeType() == AST_NODE_VARIABLE_DECLARATION) {
        std::string ret = "";
        ASTNodeVariableDeclaration * cast = (ASTNodeVariableDeclaration*)node;

        if (cast->storage != "") ret += cast->storage+" ";
        if (cast->misc != "") ret += cast->misc+" ";
        if (cast->interp != "") ret += cast->interp+" ";
        if (cast->memory != "") ret += cast->memory+" ";
        if (cast->isInvariant) ret += std::string(QUALIFIERS_INVARIANT)+" ";

        if (cast->type == "color") ret += "vec4 ";
        else ret += cast->type+" ";
        ret += cast->name;
        if (cast->arraySize > 0) ret += "["+std::to_string(cast->arraySize)+"]";
        if (cast->GetInitialValue()) {
            ret += " = ";
            ret += Node2Code(cast->GetInitialValue());
        }
        return ret;
    }
    else if (node->GetNodeType() == AST_NODE_FUNCTION) {
        std::string ret = "";
        ASTNodeFunction * cast = (ASTNodeFunction*)node;

        if (cast->storage != "") ret += cast->storage+" ";
        if (cast->misc != "") ret += cast->misc+" ";
        if (cast->interp != "") ret += cast->interp+" ";
        if (cast->memory != "") ret += cast->memory+" ";
        if (cast->isInvariant) ret += std::string(QUALIFIERS_INVARIANT)+" ";
        if (cast->returnType == "color") ret += "vec4 ";
        else ret += cast->returnType+" ";
        ret += cast->name+"(";
        for (unsigned int i = 0; i < cast->parameters.size(); ++i) {
            if (i > 0) { ret += ", "; }
            if (cast->parameters[i].qualifier != "") ret += cast->parameters[i].qualifier+" ";
            if (cast->parameters[i].type == "color") ret += "vec4 ";
            else ret += cast->parameters[i].type+" ";
            ret += cast->parameters[i].name;
            if (cast->parameters[i].arraySize > 0) ret += "["+std::to_string(cast->parameters[i].arraySize)+"]";
        }
        ret += ") {\n";
        for (unsigned int i = 0; i < cast->ChildCount(); ++i) {
            ret += Node2Code(cast->GetChild(i));
            ret += ";\n";
        }
        ret += "\n}";
        return ret;
    }
    else if (node->GetNodeType() == AST_NODE_FOR) {
        ASTNodeFor * cast = (ASTNodeFor*)node;
        std::string declares = Node2Code(cast->GetDeclares());
        std::string condition = Node2Code(cast->GetCondition());
        std::string expression = Node2Code(cast->GetExpression());
        std::string scope = Node2Code(cast->GetScope());
        return "for ("+declares+";"+condition+";"+expression+")" + scope;
    }
    else if (node->GetNodeType() == AST_NODE_WHILE) {
        ASTNodeWhile * cast = (ASTNodeWhile*)node;
        std::string condition = Node2Code(cast->GetCondition());
        std::string scope = Node2Code(cast->GetScope());
        return "while ("+condition+")" + scope;
    }
    else if (node->GetNodeType() == AST_NODE_DO) {
        ASTNodeDo * cast = (ASTNodeDo*)node;
        std::string condition = Node2Code(cast->GetCondition());
        std::string scope = Node2Code(cast->GetScope());
        return "do "+ scope + " while ("+condition+")";
    }
    else if (node->GetNodeType() == AST_NODE_SWITCH) {
        ASTNodeSwitch * cast = (ASTNodeSwitch*)node;
        std::string input = Node2Code(cast->GetInput());
        std::vector<ASTNodeCase*> cases = cast->GetCases();
        std::string ret = "switch ";
        ret += "(" + input + ") {";
        for (unsigned int i = 0; i < cases.size(); ++i) {
            ret += Node2Code(cases[i]);
            ret += "\n";
        }
        ret += "\n}";
        return ret;
    }
    else if (node->GetNodeType() == AST_NODE_CASE) {
        ASTNodeCase * cast = (ASTNodeCase*)node;
        std::string scope = Node2Code(cast->GetScope());
        if (cast->isDefault) {
            return "default: " + scope + "break;";
        }
        std::string condition = Node2Code(cast->GetCondition());
        return "case "+condition+" : " + scope + "break;";
    }
    else if (node->GetNodeType() == AST_NODE_IF) {
        ASTNodeIf * cast = (ASTNodeIf*)node;
        std::string ret = "";
        if (cast->GetCondition() != NULL) {
            ret = "if (" + Node2Code(cast->GetCondition()) + ") ";
        }
        ret += Node2Code(cast->GetScope());

        if (cast->GetNext() != NULL) {
            ret += "\nelse ";
            ret += Node2Code(cast->GetNext());
        }
        return ret;
    }
    else if (node->GetNodeType() == AST_NODE_SCOPE) {
        std::string ret = "{\n";
        for (unsigned int i = 0; i < node->ChildCount(); ++i) {
            ret += Node2Code(node->GetChild(i));
            ret += ";\n";
        }
        ret += "\n}";
        return ret;
    }
    else if (node->GetNodeType() == AST_NODE_FUNCTION_CALL) {
        ASTNodeFunctionCall * cast = (ASTNodeFunctionCall*)node;
        if (cast->function.substr(0,5) == "color") {
            if (cast->function.length() == 5) { cast->function = "vec4"; }
            else if (cast->function.length() > 5 & cast->function[5] == '[') {
                 cast->function = "vec4"+cast->function.substr(5);
            }
        }
        std::string ret = cast->function+"(";
        for (unsigned int i = 0; i < cast->ChildCount(); ++i) {
            if (i > 0) ret += ", ";
            ret += Node2Code(cast->GetChild(i));
        }
        ret += ")";

        return ret;
    }
    else if (node->GetNodeType() == AST_NODE_STRUCT) {
        ASTNodeStruct * cast = (ASTNodeStruct*)node;
        std::vector<ASTNodeField*> fields = cast->GetChildren<ASTNodeField>(AST_NODE_FIELD);
        std::string ret = "struct ";
        ret += cast->name + "{\n";
        for (unsigned int i = 0; i < fields.size(); ++i) {
            if (fields[i]->type == "color") ret += "vec4 ";
            else ret += fields[i]->type+" ";
            ret += fields[i]->name;
            if (fields[i]->arraySize > 0) ret += "["+std::to_string(fields[i]->arraySize)+"]";
            ret += ";\n";
        }
        ret += "}";
        return ret;
    }
    return "";
}
