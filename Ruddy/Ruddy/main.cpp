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

    // for (const Token& token : tokens) {
    //     std::cout << token.str() << std::endl;
    // }
    
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
struct Expression {
    
};

std::vector<Expression> parseLine(const std::vector<Token> tokenLine) {
    std::vector<Expression> expressions;
    return expressions;
}

std::vector<std::vector<Expression>> parse(const std::vector<std::vector<Token>>& tokenLines) {
    std::vector<std::vector<Expression>> expressions;
    for (const std::vector<Token>& tokenLine : tokenLines) {
        expressions.push_back(parseLine(tokenLine));
    }
    return expressions;
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
    
    return 0;
}
