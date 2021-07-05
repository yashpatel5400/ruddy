#ifndef lexer_hpp
#define lexer_hpp

#include <stdio.h>

#include <string>
#include <vector>

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
    
    std::string str() const;
};

std::vector<std::vector<Token>> tokenize(const std::vector<std::string>& lines);

#endif /* lexer_hpp */
