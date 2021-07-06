#ifndef parser_hpp
#define parser_hpp

#include <stdio.h>

#include <map>
#include <string>
#include <vector>

#include "lexer.hpp"

enum class ExpressionType {
    VALUE,
    ADD,
    SUB,
    MUL,
    DIV,
    PAREN,
    VAR,
    STRING,
    PRINT,
    IF,
    IS_LESS,
    IS_LEQ,
    IS_GREATER,
    IS_GEQ,
    IS_EQ,
};

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
    
    // control flow
    std::vector<std::vector<std::shared_ptr<Expression>>> ifStatements;
    std::vector<std::vector<std::shared_ptr<Expression>>> elseStatements;
    
    std::shared_ptr<Expression> conditional;
    
    Expression() {}
    
    std::string str() const;
};

std::vector<std::shared_ptr<Expression>> wrapTokens(const std::vector<Token> tokens);
void parse(
           std::map<std::string, std::vector<std::vector<std::shared_ptr<Expression>>>>& funcExpressions,
           const std::vector<std::vector<std::shared_ptr<Expression>>>& expressionLines);

#endif /* parser_hpp */
