#include <iostream>
#include <fstream>
#include <map>

#include <gflags/gflags.h>

DEFINE_string(input_path, "", "Path to test file");

std::map<std::string, int> intVariables;
std::map<std::string, std::string> strVariables;

// --- Lexer
enum class TokenType {
    ADD,
    SUB,
    MUL,
    DIV,
    EQUAL,
    LEFT_PAREN,
    RIGHT_PAREN,
    DOUBLE_QUOTE,
    SINGLE_QUOTE,
    WORD,
    NUMBER
};

std::string printTokenType(TokenType tokenType) {
         if (tokenType == TokenType::ADD) { return "ADD"; }
    else if (tokenType == TokenType::SUB) { return "SUB"; }
    else if (tokenType == TokenType::MUL) { return "MUL"; }
    else if (tokenType == TokenType::DIV) { return "DIV"; }
    else if (tokenType == TokenType::EQUAL) { return "EQUAL"; }
    else if (tokenType == TokenType::LEFT_PAREN) { return "LEFT_PAREN"; }
    else if (tokenType == TokenType::RIGHT_PAREN) { return "RIGHT_PAREN"; }
    else if (tokenType == TokenType::DOUBLE_QUOTE) { return "DOUBLE_QUOTE"; }
    else if (tokenType == TokenType::SINGLE_QUOTE) { return "SINGLE_QUOTE"; }
    else if (tokenType == TokenType::WORD) { return "WORD"; }
    else if (tokenType == TokenType::NUMBER) { return "NUMBER"; }
    else { return "INVALID_TOKEN"; }
}

struct Token {
    TokenType tokenType;
    std::string payload;

    Token() : payload("") {}
    
    Token(const std::string& s) : payload(s) {
             if (s == "+")  { tokenType = TokenType::ADD; }
        else if (s == "-")  { tokenType = TokenType::SUB; }
        else if (s == "*")  { tokenType = TokenType::MUL; }
        else if (s == "/")  { tokenType = TokenType::DIV; }
        else if (s == "=")  { tokenType = TokenType::EQUAL; }
        else if (s == "(")  { tokenType = TokenType::LEFT_PAREN; }
        else if (s == ")")  { tokenType = TokenType::RIGHT_PAREN; }
        else if (s == "\"") { tokenType = TokenType::DOUBLE_QUOTE; }
        else if (s == "'")  { tokenType = TokenType::SINGLE_QUOTE; }
        else if (std::isdigit(s[0])) { tokenType = TokenType::NUMBER; }
        else { tokenType = TokenType::WORD; }
    }
    
    std::string str() const {
        return printTokenType(tokenType) + " - " + payload;
    }
};

std::vector<Token> tokenize_line(const std::string& line) {
    std::vector<Token> tokens;
    
    std::string curPayload;
    bool inString = false;
    for (const char c : line) {
        if (
            (!inString && (c == ' ' || c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')' || c == '\"' || c == '\'')) ||
            (inString  && (c == '\"' || c == '\''))
           ) {
            if (curPayload.size() > 0) {
                tokens.push_back(Token(curPayload));
                curPayload = std::string();
            }
            
            if (c != ' ') {
                std::string temp;
                temp += c;
                tokens.push_back(Token(temp));
            }
            
            if (c == '\"' || c == '\'') {
                inString = !inString;
            }
        } else {
            curPayload += c;
        }
    }
    
    if (curPayload.size() > 0) {
        tokens.push_back(Token(curPayload));
    }

    return tokens;
}

std::vector<std::vector<Token>> tokenize(const std::vector<std::string>& lines) {
    std::vector<std::vector<Token>> statements;
    for (const std::string& line : lines) {
        statements.push_back(tokenize_line(line));
    }
    return statements;
}

// --- Parser
/*
 * Expression BNF are assumed to take the following forms (and parsed in the order listed):
 * expression  : []
 * value       : [value]
 * paren       : ([value])
 * mul/div     : [value] (*,/) [value]
 * add/sub     : [value] (+,-) [value]
 */
struct Expression;
std::vector<std::shared_ptr<Expression>> parseLine(const std::vector<std::shared_ptr<Expression>> expressions);

enum class ExpressionType {
    VALUE,
    ADD,
    SUB,
    MUL,
    DIV,
    PAREN,
    VAR,
    STRING,
};

std::string printExpressionType(ExpressionType tokenType) {
         if (tokenType == ExpressionType::VALUE)  { return "VALUE"; }
    else if (tokenType == ExpressionType::ADD)    { return "ADD"; }
    else if (tokenType == ExpressionType::SUB)    { return "SUB"; }
    else if (tokenType == ExpressionType::MUL)    { return "MUL"; }
    else if (tokenType == ExpressionType::DIV)    { return "DIV"; }
    else if (tokenType == ExpressionType::PAREN)  { return "PAREN"; }
    else if (tokenType == ExpressionType::PAREN)  { return "VAR"; }
    else if (tokenType == ExpressionType::STRING) { return "STRING"; }
    else { return "INVALID_EXPRESSION"; }
}

// NOTE: not the best design, but we just have a single expression and just use the subset of
// properties that are relevant to each expression type
struct Expression {
    ExpressionType expressionType;
    
    // value tokens
    Token token;
    
    // variable setting
    std::shared_ptr<Expression> payload;
    
    // variable setting
    std::shared_ptr<Expression> var;
    
    // unary ops
    std::vector<std::shared_ptr<Expression>> core;
    
    // binary ops
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
    
    Expression() {}
    
    std::string str() const {
        if (expressionType == ExpressionType::VALUE)      { return "<" + token.str() + ">"; }
        else if (expressionType == ExpressionType::PAREN) {
            std::string representation;
            representation = "(";
            for (std::shared_ptr<Expression> expression : core) {
                representation += expression->str();
            }
            representation += ")";
            return representation;
        }
        else if (expressionType == ExpressionType::MUL)   { return "<" + left->str() + "> * <" + right->str() + ">"; }
        else if (expressionType == ExpressionType::DIV)   { return "<" + left->str() + "> / <" + right->str() + ">"; }
        else if (expressionType == ExpressionType::ADD)   { return "<" + left->str() + "> + <" + right->str() + ">"; }
        else if (expressionType == ExpressionType::SUB)   { return "<" + left->str() + "> - <" + right->str() + ">"; }
        else if (expressionType == ExpressionType::STRING)   { return "\"" + payload->token.payload + "\""; }
        else if (expressionType == ExpressionType::VAR)   {
            std::string representation;
            representation += var->str();
            representation += " = ";
            for (std::shared_ptr<Expression> expression : core) {
                representation += expression->str();
            }
            return representation;
        }
        else { return "INVALID_EXPRESSION"; }
    }
};

std::shared_ptr<Expression> valueExpression(Token token) {
    std::shared_ptr<Expression> expression = std::make_shared<Expression>();
    expression->expressionType = ExpressionType::VALUE;
    expression->token = token;
    return expression;
}

std::shared_ptr<Expression> binaryOpExpression(ExpressionType expressionType, std::shared_ptr<Expression> core, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right) {
    std::shared_ptr<Expression> expression = std::make_shared<Expression>();
    expression->token = core->token;
    expression->expressionType = expressionType;
    expression->left = left;
    expression->right = right;
    return expression;
}

std::shared_ptr<Expression> parenExpression(std::shared_ptr<Expression> root, std::vector<std::shared_ptr<Expression>> core) {
    std::shared_ptr<Expression> expression = std::make_shared<Expression>();
    expression->expressionType = ExpressionType::PAREN;
    expression->token = root->token;
    expression->core = parseLine(core);
    return expression;
}

std::shared_ptr<Expression> varExpression(std::shared_ptr<Expression> root, std::shared_ptr<Expression> var, std::vector<std::shared_ptr<Expression>> core) {
    std::shared_ptr<Expression> expression = std::make_shared<Expression>();
    expression->token = root->token;
    expression->var = var;
    expression->expressionType = ExpressionType::VAR;
    expression->core = parseLine(core);
    return expression;
}

std::shared_ptr<Expression> strExpression(std::shared_ptr<Expression> root, std::shared_ptr<Expression> payload) {
    std::shared_ptr<Expression> expression = std::make_shared<Expression>();
    expression->expressionType = ExpressionType::STRING;
    expression->token = root->token;
    expression->payload = payload;
    return expression;
}

std::vector<std::shared_ptr<Expression>> valueExpressions(const std::vector<Token> tokens) {
    std::vector<std::shared_ptr<Expression>> expressions;
    for (Token token : tokens) {
        expressions.push_back(valueExpression(token));
    }
    return expressions;
}

std::vector<std::shared_ptr<Expression>> binaryOpExpressions(const std::vector<std::shared_ptr<Expression>> expressions, std::vector<ExpressionType> expressionTypes) {
    
    std::vector<TokenType> tokenTypes;
    for (ExpressionType expressionType : expressionTypes) {
        if (expressionType == ExpressionType::ADD) { tokenTypes.push_back(TokenType::ADD); }
        if (expressionType == ExpressionType::SUB) { tokenTypes.push_back(TokenType::SUB); }
        if (expressionType == ExpressionType::MUL) { tokenTypes.push_back(TokenType::MUL); }
        if (expressionType == ExpressionType::DIV) { tokenTypes.push_back(TokenType::DIV); }
    }
    
    std::vector<std::shared_ptr<Expression>> newExpressions;
    bool addingExpression = false;
    ExpressionType expressionType;
    for (std::shared_ptr<Expression> expression : expressions) {
        if (addingExpression) {
            std::shared_ptr<Expression> core = newExpressions.back();
            newExpressions.pop_back();
            std::shared_ptr<Expression> left = newExpressions.back();
            newExpressions.pop_back();
            newExpressions.push_back(binaryOpExpression(expressionType, core, left, expression));
            addingExpression = false;
        } else {
            for (TokenType tokenType : tokenTypes) {
                if (expression->token.tokenType == tokenType) {
                    addingExpression = true;
                    if (tokenType == TokenType::ADD) { expressionType = ExpressionType::ADD; }
                    if (tokenType == TokenType::SUB) { expressionType = ExpressionType::SUB; }
                    if (tokenType == TokenType::MUL) { expressionType = ExpressionType::MUL; }
                    if (tokenType == TokenType::DIV) { expressionType = ExpressionType::DIV; }
                }
            }
            
            newExpressions.push_back(expression);
        }
    }
    return newExpressions;
}

std::vector<std::shared_ptr<Expression>> parenExpressions(const std::vector<std::shared_ptr<Expression>> expressions) {
    std::vector<std::shared_ptr<Expression>> newExpressions;
    bool addingExpression = false;
    std::vector<std::shared_ptr<Expression>> currentParenExpression;
    for (std::shared_ptr<Expression> expression : expressions) {
        if (addingExpression) {
            if (expression->token.tokenType == TokenType::RIGHT_PAREN) {
                newExpressions.push_back(parenExpression(expression, currentParenExpression));
                addingExpression = false;
            } else {
                currentParenExpression.push_back(expression);
            }
        } else {
            if (expression->token.tokenType == TokenType::LEFT_PAREN) {
                addingExpression = true;
                currentParenExpression = std::vector<std::shared_ptr<Expression>>();
            } else {
                newExpressions.push_back(expression);
            }
        }
    }
    return newExpressions;
}

std::vector<std::shared_ptr<Expression>> varExpressions(const std::vector<std::shared_ptr<Expression>> expressions) {
    std::vector<std::shared_ptr<Expression>> newExpressions;
    bool isVarExpr = false;
    std::vector<std::shared_ptr<Expression>> varExpr;
    for (std::shared_ptr<Expression> expression : expressions) {
        if (isVarExpr) {
            varExpr.push_back(expression);
        } else {
            if (expression->token.tokenType == TokenType::EQUAL) {
                isVarExpr = true;
            }
            newExpressions.push_back(expression);
        }
    }
    
    if (isVarExpr) {
        std::shared_ptr<Expression> core = newExpressions.back();
        newExpressions.pop_back();
        std::shared_ptr<Expression> var = newExpressions.back();
        newExpressions.pop_back();
        newExpressions.push_back(varExpression(core, var, varExpr));
    }
        
    return newExpressions;
}

std::vector<std::shared_ptr<Expression>> stringExpressions(const std::vector<std::shared_ptr<Expression>> expressions) {
    std::vector<std::shared_ptr<Expression>> newExpressions;
    bool isStrExpr = false;
    std::vector<std::shared_ptr<Expression>> strExpr;
    for (std::shared_ptr<Expression> expression : expressions) {
        if (isStrExpr) {
            if (expression->token.tokenType == TokenType::DOUBLE_QUOTE) {
                // strExpr should always only be 1 long
                newExpressions.push_back(strExpression(expression, strExpr[0]));
                isStrExpr = false;
            } else {
                strExpr.push_back(expression);
            }
        } else {
            if (expression->token.tokenType == TokenType::DOUBLE_QUOTE) {
                isStrExpr = true;
                strExpr = std::vector<std::shared_ptr<Expression>>();
            } else {
                newExpressions.push_back(expression);
            }
        }
    }
    
    return newExpressions;
}

std::vector<std::shared_ptr<Expression>> parseLine(const std::vector<std::shared_ptr<Expression>> expressions) {
    std::vector<std::shared_ptr<Expression>> newExpressions = expressions;
    
    newExpressions = varExpressions(newExpressions);
    
    newExpressions = stringExpressions(newExpressions);
    
    newExpressions = parenExpressions(newExpressions);

    newExpressions = binaryOpExpressions(newExpressions, {ExpressionType::MUL, ExpressionType::DIV});
    newExpressions = binaryOpExpressions(newExpressions, {ExpressionType::ADD, ExpressionType::SUB});
    
    return newExpressions;
}

std::vector<std::vector<std::shared_ptr<Expression>>> parse(const std::vector<std::vector<std::shared_ptr<Expression>>>& expressionLines) {
    std::vector<std::vector<std::shared_ptr<Expression>>> expressions;
    for (const std::vector<std::shared_ptr<Expression>>& expressionLine : expressionLines) {
        expressions.push_back(parseLine(expressionLine));
    }
    return expressions;
}

// --- Evaluator
int evaluateLine(const std::vector<std::shared_ptr<Expression>>& expressionLine) {
    for (const std::shared_ptr<Expression>& expression : expressionLine) {
        switch(expression->expressionType) {
            case ExpressionType::VALUE: {
                if (intVariables.find(expression->token.payload) != intVariables.end()) {
                    return intVariables[expression->token.payload];
                }
                return std::stoi(expression->token.payload);
            }
            case ExpressionType::ADD:    { return evaluateLine({expression->left}) + evaluateLine({expression->right}); }
            case ExpressionType::SUB:    { return evaluateLine({expression->left}) - evaluateLine({expression->right}); }
            case ExpressionType::MUL:    { return evaluateLine({expression->left}) * evaluateLine({expression->right}); }
            case ExpressionType::DIV:    { return evaluateLine({expression->left}) / evaluateLine({expression->right}); }
            case ExpressionType::PAREN:  { return evaluateLine(expression->core); }
            case ExpressionType::STRING: {
                std::cout << expression->payload->token.payload << std::endl;
                return 0;
            }
            case ExpressionType::VAR:    {
                intVariables[expression->var->token.payload] = evaluateLine(expression->core);
                return 0;
            }
            default: break;
        }
    }
    
    return 0;
}

void evalute(const std::vector<std::vector<std::shared_ptr<Expression>>>& expressions) {
    for (const std::vector<std::shared_ptr<Expression>>& expressionLine : expressions) {
        std::cout << evaluateLine(expressionLine) << std::endl;
    }
    
    std::cout << "--- variables ---" << std::endl;
    std::map<std::string, int>::iterator it;
    for (it = intVariables.begin(); it != intVariables.end(); it++)
    {
        std::cout << it->first    // string (key)
                  << ':'
                  << it->second   // string's value
                  << std::endl;
    }
}

// --- Tester
int main(int argc, char * argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    std::vector<std::string> lines;
    
    std::ifstream s;
    s.open(FLAGS_input_path);
    for (std::string line; std::getline(s, line); ) {
        lines.push_back(line);
    }
    s.close();
    
    // basic flow: strings -> tokens -> expressions -> values
    std::vector<std::vector<Token>> tokens = tokenize(lines);
    
    // box up tokens as expressions and do parsing
    std::vector<std::vector<std::shared_ptr<Expression>>> expressions;
    for (const std::vector<Token>& tokenLine : tokens) {
        // for (Token token : tokenLine) {
        //     std::cout << token.payload << " " << printTokenType(token.tokenType) << std::endl;
        // }
        
        expressions.push_back(valueExpressions(tokenLine));
    }
    std::vector<std::vector<std::shared_ptr<Expression>>> parsedExpressions = parse(expressions);
    
    // for (std::shared_ptr<Expression> expression : parsedExpressions[0]) {
    //     std::cout << expression->str() << std::endl;
    // }
    
    evalute(parsedExpressions);
    
    return 0;
}
