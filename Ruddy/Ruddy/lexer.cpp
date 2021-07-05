#include "lexer.hpp"

#include <iostream>

std::string printTokenType(TokenType tokenType) {
         if (tokenType == TokenType::ADD) { return "ADD"; }
    else if (tokenType == TokenType::SUB) { return "SUB"; }
    else if (tokenType == TokenType::MUL) { return "MUL"; }
    else if (tokenType == TokenType::DIV) { return "DIV"; }
    else if (tokenType == TokenType::IS_LESS) { return "IS_LESS"; }
    else if (tokenType == TokenType::IS_LEQ) { return "IS_LEQ"; }
    else if (tokenType == TokenType::IS_GREATER) { return "IS_GREATER"; }
    else if (tokenType == TokenType::IS_GEQ) { return "IS_GEQ"; }
    else if (tokenType == TokenType::IS_EQ) { return "IS_EQ"; }
    else if (tokenType == TokenType::EQUAL) { return "EQUAL"; }
    else if (tokenType == TokenType::LEFT_PAREN) { return "LEFT_PAREN"; }
    else if (tokenType == TokenType::RIGHT_PAREN) { return "RIGHT_PAREN"; }
    else if (tokenType == TokenType::DOUBLE_QUOTE) { return "DOUBLE_QUOTE"; }
    else if (tokenType == TokenType::SINGLE_QUOTE) { return "SINGLE_QUOTE"; }
    else if (tokenType == TokenType::WORD) { return "WORD"; }
    else if (tokenType == TokenType::NUMBER) { return "NUMBER"; }
    else { return "INVALID_TOKEN"; }
}

std::string Token::str() const {
   return printTokenType(tokenType) + " - " + payload;
}

std::vector<Token> tokenize_line(const std::string& line) {
    std::vector<Token> tokens;
    
    std::string curPayload;
    bool inString = false;
    
    int strIdx = 0;
    while(strIdx < line.size()) {
        char c = line[strIdx];
        if (
            (!inString && (c == ' ' || c == '>' || c == '<' || c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')' || c == '\"' || c == '\'')) ||
            (inString  && (c == '\"' || c == '\''))
           ) {
            if (curPayload.size() > 0) {
                tokens.push_back(Token(curPayload));
                curPayload = std::string();
            }
            
            if (c == '/' && line[strIdx + 1] == '/') {
                return tokens; // end of line, since rest is comment
            }
            
            if (c != ' ') {
                std::string temp;
                temp += c;
                
                if ((c == '>' || c == '<' || c == '=') && line[strIdx + 1] == '=') {
                    temp += line[strIdx + 1];
                    strIdx++;
                }
                
                tokens.push_back(Token(temp));
            }
            
            if (c == '\"' || c == '\'') {
                inString = !inString;
            }
        } else {
            curPayload += c;
        }
        
        strIdx++;
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
