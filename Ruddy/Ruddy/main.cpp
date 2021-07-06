#include <iostream>
#include <fstream>
#include <map>

#include <gflags/gflags.h>

#include "lexer.hpp"
#include "parser.hpp"

DEFINE_string(input_path, "", "Path to test file");

std::map<std::string, int> intVariables;
std::map<std::string, std::string> strVariables;
std::map<std::string, std::vector<std::vector<std::shared_ptr<Expression>>>> funcExpressions;

// --- Evaluator
// err, is this the best way? could use union but meh
enum class ResultType {
    INT,
    STR,
    NONE
};

struct Result {
    int resultInt;
    std::string resultStr;
    ResultType resultType; // says which to reference
};

std::string resultTypeToStr(ResultType resultType) {
    switch(resultType) {
        case ResultType::INT:  { return "INT"; }
        case ResultType::STR:  { return "STR"; }
        case ResultType::NONE: { return "NONE"; }
    }
    return "";
}

bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

void evaluate(const std::vector<std::vector<std::shared_ptr<Expression>>>& expressions);

Result evaluateLine(const std::vector<std::shared_ptr<Expression>>& expressionLine) {
    Result result;
    result.resultType = ResultType::NONE;
    
    for (const std::shared_ptr<Expression>& expression : expressionLine) {
        switch(expression->expressionType) {
            case ExpressionType::VALUE: {
                if (intVariables.find(expression->token.payload) != intVariables.end()) {
                    result.resultType = ResultType::INT;
                    result.resultInt  = intVariables[expression->token.payload];
                } else if (strVariables.find(expression->token.payload) != strVariables.end()) {
                    result.resultType = ResultType::STR;
                    result.resultStr  = strVariables[expression->token.payload];
                } else if (funcExpressions.find(expression->token.payload) != funcExpressions.end()) {
                    evaluate(funcExpressions[expression->token.payload]);
                } else {
                    if (is_number(expression->token.payload)) {
                        result.resultType = ResultType::INT;
                        result.resultInt  = std::stoi(expression->token.payload);
                    } else {
                        result.resultType = ResultType::STR;
                        result.resultStr  = expression->token.payload;
                    }
                }
                break;
            }
            case ExpressionType::ADD: {
                Result addResultLeft = evaluateLine({expression->left});
                Result addResultRight = evaluateLine({expression->right});
                
                if (addResultLeft.resultType == ResultType::INT) {
                    result.resultType = ResultType::INT;
                    result.resultInt  = addResultLeft.resultInt + addResultRight.resultInt;
                } else {
                    result.resultType = ResultType::STR;
                    result.resultStr  = addResultLeft.resultStr + addResultRight.resultStr;
                }
                break;
            }
            case ExpressionType::SUB: {
                result.resultType = ResultType::INT;
                result.resultInt  = evaluateLine({expression->left}).resultInt - evaluateLine({expression->right}).resultInt;
                break;
            }
            case ExpressionType::MUL: {
                result.resultType = ResultType::INT;
                result.resultInt  = evaluateLine({expression->left}).resultInt * evaluateLine({expression->right}).resultInt;
                break;
            }
            case ExpressionType::DIV: {
                result.resultType = ResultType::INT;
                result.resultInt  = evaluateLine({expression->left}).resultInt / evaluateLine({expression->right}).resultInt;
                break;
            }
            case ExpressionType::IF: {
                Result boolResult = evaluateLine({expression->conditional});
                if (boolResult.resultInt) {
                    evaluate(expression->ifStatements);
                }
                break;
            }
            case ExpressionType::IS_LESS: {
                result.resultType = ResultType::INT;
                result.resultInt  = evaluateLine({expression->left}).resultInt < evaluateLine({expression->right}).resultInt;
                break;
            }
            case ExpressionType::IS_LEQ: {
                result.resultType = ResultType::INT;
                result.resultInt  = evaluateLine({expression->left}).resultInt <= evaluateLine({expression->right}).resultInt;
                break;
            }
            case ExpressionType::IS_GREATER: {
                result.resultType = ResultType::INT;
                result.resultInt  = evaluateLine({expression->left}).resultInt > evaluateLine({expression->right}).resultInt;
                break;
            }
            case ExpressionType::IS_GEQ: {
                result.resultType = ResultType::INT;
                result.resultInt  = evaluateLine({expression->left}).resultInt >= evaluateLine({expression->right}).resultInt;
                break;
            }
            case ExpressionType::IS_EQ: {
                result.resultType = ResultType::INT;
                result.resultInt  = evaluateLine({expression->left}).resultInt == evaluateLine({expression->right}).resultInt;
                break;
            }
            case ExpressionType::PAREN:  {
                return evaluateLine(expression->core);
            }
            case ExpressionType::PRINT:  {
                Result printExprResult = evaluateLine(expression->core);
                if (printExprResult.resultType == ResultType::INT) {
                    std::cout << printExprResult.resultInt << std::endl;
                } else {
                    std::cout << printExprResult.resultStr << std::endl;
                }
                break;
            }
            case ExpressionType::STRING: {
                result.resultType = ResultType::STR;
                result.resultStr  = expression->payload->token.payload;
                break;
            }
            case ExpressionType::VAR: {
                Result varResult = evaluateLine(expression->core);
                if (varResult.resultType == ResultType::INT) {
                    intVariables[expression->var->token.payload] = varResult.resultInt;
                } else {
                    strVariables[expression->var->token.payload] = varResult.resultStr;
                }

                break;
            }
            default: break;
        }
    }
    
    return result;
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
    parse(funcExpressions, expressions);
    
    // for (std::vector<std::shared_ptr<Expression>> expressions : funcExpressions["main"]) {
    //     for (std::shared_ptr<Expression> expression : expressions) {
    //         std::cout << expression->str() << std::endl;
    //     }
    // }
    
    evaluate(funcExpressions["main"]);
    
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
