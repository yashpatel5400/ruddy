#include <iostream>
#include <fstream>
#include <map>

#include <gflags/gflags.h>

#include "lexer.hpp"
#include "parser.hpp"

DEFINE_string(input_path, "", "Path to test file");

std::map<std::string, int> intVariables;
std::map<std::string, std::string> strVariables;

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
            case ExpressionType::PRINT:  {
                std::cout << evaluateLine(expression->core) << std::endl;
                return 0;
            }
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

void evaluate(const std::vector<std::vector<std::shared_ptr<Expression>>>& expressions) {
    for (const std::vector<std::shared_ptr<Expression>>& expressionLine : expressions) {
        evaluateLine(expressionLine);
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
        
        expressions.push_back(wrapTokens(tokenLine));
    }
    std::vector<std::vector<std::shared_ptr<Expression>>> parsedExpressions = parse(expressions);
    
    // for (std::shared_ptr<Expression> expression : parsedExpressions[0]) {
    //     std::cout << expression->str() << std::endl;
    // }
    
    evaluate(parsedExpressions);
    
    // std::cout << "--- variables ---" << std::endl;
    // std::map<std::string, int>::iterator it;
    // for (it = intVariables.begin(); it != intVariables.end(); it++)
    // {
    //     std::cout << it->first    // string (key)
    //               << ':'
    //               << it->second   // string's value
    //               << std::endl;
    // }
    
    return 0;
}
