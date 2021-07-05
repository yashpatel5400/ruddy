#include "parser.hpp"

std::vector<std::shared_ptr<Expression>> parseLine(const std::vector<std::shared_ptr<Expression>> expressions);

std::string printExpressionType(ExpressionType tokenType) {
         if (tokenType == ExpressionType::VALUE)  { return "VALUE"; }
    else if (tokenType == ExpressionType::ADD)    { return "ADD"; }
    else if (tokenType == ExpressionType::SUB)    { return "SUB"; }
    else if (tokenType == ExpressionType::MUL)    { return "MUL"; }
    else if (tokenType == ExpressionType::DIV)    { return "DIV"; }
    else if (tokenType == ExpressionType::PAREN)  { return "PAREN"; }
    else if (tokenType == ExpressionType::PAREN)  { return "VAR"; }
    else if (tokenType == ExpressionType::STRING) { return "STRING"; }
    else if (tokenType == ExpressionType::PRINT)  { return "PRINT"; }
    else { return "INVALID_EXPRESSION"; }
}

std::string Expression::str() const {
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
    else if (expressionType == ExpressionType::STRING)   { return "print(" + payload->token.payload + ")"; }
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

std::shared_ptr<Expression> printExpression(std::shared_ptr<Expression> root, std::vector<std::shared_ptr<Expression>> core) {
    std::shared_ptr<Expression> expression = std::make_shared<Expression>();
    expression->expressionType = ExpressionType::PRINT;
    expression->token = root->token;
    expression->core = parseLine(core);
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

std::vector<std::shared_ptr<Expression>> wrapTokens(const std::vector<Token> tokens) {
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

std::vector<std::shared_ptr<Expression>> printExpressions(const std::vector<std::shared_ptr<Expression>> expressions) {
    std::vector<std::shared_ptr<Expression>> newExpressions;
    bool isPrintExpr = false;
    std::vector<std::shared_ptr<Expression>> printExpr;
    std::shared_ptr<Expression> rootExpr;
    for (std::shared_ptr<Expression> expression : expressions) {
        if (isPrintExpr) {
            printExpr.push_back(expression);
        } else {
            if (expression->token.tokenType == TokenType::WORD && expression->token.payload == "print") {
                rootExpr = expression;
                isPrintExpr = true;
            } else {
                newExpressions.push_back(expression);
            }
        }
    }
    
    if (isPrintExpr) {
        newExpressions.push_back(printExpression(rootExpr, printExpr));
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
    
    newExpressions = printExpressions(newExpressions);
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
