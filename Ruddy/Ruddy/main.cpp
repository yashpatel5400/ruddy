#include <iostream>
#include <fstream>

#include <gflags/gflags.h>

DEFINE_string(input_path, "", "Path to test file");

// --- Lexer
enum class TokenType {
    ADD,
    SUB,
    MUL,
    DIV,
    LEFT_PAREN,
    RIGHT_PAREN,
    WORD,
    NUMBER
};

std::string printTokenType(TokenType tokenType) {
         if (tokenType == TokenType::ADD) { return "ADD"; }
    else if (tokenType == TokenType::SUB) { return "SUB"; }
    else if (tokenType == TokenType::MUL) { return "MUL"; }
    else if (tokenType == TokenType::DIV) { return "DIV"; }
    else if (tokenType == TokenType::LEFT_PAREN) { return "LEFT_PAREN"; }
    else if (tokenType == TokenType::RIGHT_PAREN) { return "RIGHT_PAREN"; }
    else if (tokenType == TokenType::WORD) { return "WORD"; }
    else if (tokenType == TokenType::NUMBER) { return "NUMBER"; }
    else { return "INVALID_TOKEN"; }
}

struct Token {
    TokenType tokenType;
    std::string payload;

    Token() : payload("") {}
    
    Token(const std::string& s) : payload(s) {
             if (s == "+") { tokenType = TokenType::ADD; }
        else if (s == "-") { tokenType = TokenType::SUB; }
        else if (s == "*") { tokenType = TokenType::MUL; }
        else if (s == "/") { tokenType = TokenType::DIV; }
        else if (s == "(") { tokenType = TokenType::LEFT_PAREN; }
        else if (s == ")") { tokenType = TokenType::RIGHT_PAREN; }
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
    for (const char c : line) {
        if (c == ' ' || (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')')) {
            if (curPayload.size() > 0) {
                tokens.push_back(Token(curPayload));
                curPayload = std::string();
            }
            
            if (c != ' ') {
                std::string temp;
                temp += c;
                tokens.push_back(Token(temp));
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
};

std::string printExpressionType(ExpressionType tokenType) {
         if (tokenType == ExpressionType::VALUE) { return "VALUE"; }
    else if (tokenType == ExpressionType::ADD)   { return "ADD"; }
    else if (tokenType == ExpressionType::SUB)   { return "SUB"; }
    else if (tokenType == ExpressionType::MUL)   { return "MUL"; }
    else if (tokenType == ExpressionType::DIV)   { return "DIV"; }
    else if (tokenType == ExpressionType::PAREN) { return "PAREN"; }
    else { return "INVALID_EXPRESSION"; }
}

// NOTE: not the best design, but we just have a single expression and just use the subset of
// properties that are relevant to each expression type
struct Expression {
    ExpressionType expressionType;
    
    // value tokens
    Token token;
    
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
                std::cout << " ----- " << std::endl;
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

std::vector<std::shared_ptr<Expression>> parseLine(const std::vector<std::shared_ptr<Expression>> expressions) {
    std::vector<std::shared_ptr<Expression>> newExpressions = expressions;

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
            case ExpressionType::VALUE: { return std::stoi(expression->token.payload); }
            case ExpressionType::ADD:   { return evaluateLine({expression->left}) + evaluateLine({expression->right}); }
            case ExpressionType::SUB:   { return evaluateLine({expression->left}) - evaluateLine({expression->right}); }
            case ExpressionType::MUL:   { return evaluateLine({expression->left}) * evaluateLine({expression->right}); }
            case ExpressionType::DIV:   { return evaluateLine({expression->left}) / evaluateLine({expression->right}); }
            case ExpressionType::PAREN: { return 0; }
            default: break;
        }
    }
    
    return 0;
}

void evalute(const std::vector<std::vector<std::shared_ptr<Expression>>>& expressions) {
    for (const std::vector<std::shared_ptr<Expression>>& expressionLine : expressions) {
        std::cout << evaluateLine(expressionLine) << std::endl;
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
        expressions.push_back(valueExpressions(tokenLine));
    }
    std::vector<std::vector<std::shared_ptr<Expression>>> parsedExpressions = parse(expressions);
    
    evalute(parsedExpressions);
    
    return 0;
}
