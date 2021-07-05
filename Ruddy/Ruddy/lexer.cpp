#include "lexer.hpp"

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

std::string Token::str() const {
   return printTokenType(tokenType) + " - " + payload;
}

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
