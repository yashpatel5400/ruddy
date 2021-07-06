#include "parser.hpp"

#include <iostream>

std::vector<std::shared_ptr<Expression>> parseLine(const std::vector<std::shared_ptr<Expression>> expressions);

std::string printExpressionType(ExpressionType tokenType) {
         if (tokenType == ExpressionType::VALUE)      { return "VALUE"; }
    else if (tokenType == ExpressionType::ADD)        { return "ADD"; }
    else if (tokenType == ExpressionType::SUB)        { return "SUB"; }
    else if (tokenType == ExpressionType::MUL)        { return "MUL"; }
    else if (tokenType == ExpressionType::DIV)        { return "DIV"; }
    else if (tokenType == ExpressionType::IF)         { return "IF"; }
    else if (tokenType == ExpressionType::IS_LESS)    { return "IS_LESS"; }
    else if (tokenType == ExpressionType::IS_LEQ)     { return "IS_LEQ"; }
    else if (tokenType == ExpressionType::IS_GREATER) { return "IS_GREATER"; }
    else if (tokenType == ExpressionType::IS_GEQ)     { return "IS_GEQ"; }
    else if (tokenType == ExpressionType::IS_EQ)      { return "IS_EQ"; }
    else if (tokenType == ExpressionType::PAREN)      { return "PAREN"; }
    else if (tokenType == ExpressionType::PAREN)      { return "VAR"; }
    else if (tokenType == ExpressionType::STRING)     { return "STRING"; }
    else if (tokenType == ExpressionType::PRINT)      { return "PRINT"; }
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
    else if (expressionType == ExpressionType::MUL)        { return "<" + left->str() + "> * <" + right->str() + ">";  }
    else if (expressionType == ExpressionType::DIV)        { return "<" + left->str() + "> / <" + right->str() + ">";  }
    else if (expressionType == ExpressionType::ADD)        { return "<" + left->str() + "> + <" + right->str() + ">";  }
    else if (expressionType == ExpressionType::SUB)        { return "<" + left->str() + "> - <" + right->str() + ">";  }
    else if (expressionType == ExpressionType::IF)         {
        std::string representation;
        representation = "if ";
        representation += conditional->str();
        representation += '\n';
        for (std::vector<std::shared_ptr<Expression>> expressions : ifStatements) {
            for (std::shared_ptr<Expression> expression : expressions) {
                representation += expression->str();
            }
            representation += '\n';
        }
        
        return representation;
    }
    else if (expressionType == ExpressionType::IS_LESS)    { return "<" + left->str() + "> < <" + right->str() + ">";  }
    else if (expressionType == ExpressionType::IS_LEQ)     { return "<" + left->str() + "> <= <" + right->str() + ">"; }
    else if (expressionType == ExpressionType::IS_GREATER) { return "<" + left->str() + "> > <" + right->str() + ">";  }
    else if (expressionType == ExpressionType::IS_GEQ)     { return "<" + left->str() + "> >= <" + right->str() + ">"; }
    else if (expressionType == ExpressionType::IS_EQ)      { return "<" + left->str() + "> == <" + right->str() + ">"; }
    else if (expressionType == ExpressionType::STRING)     { return "\"" + payload->token.payload + "\""; }
    else if (expressionType == ExpressionType::PRINT)      { return "print(" + printExpressionType(core[0]->expressionType) + ")"; }
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

std::shared_ptr<Expression> ifExpression(std::shared_ptr<Expression> root, std::shared_ptr<Expression> conditional, std::vector<std::vector<std::shared_ptr<Expression>>> ifStatements, std::vector<std::vector<std::shared_ptr<Expression>>> elseStatements) {
    std::shared_ptr<Expression> expression = std::make_shared<Expression>();
    expression->expressionType = ExpressionType::IF;
    expression->token = root->token;
    expression->conditional = conditional;
    expression->ifStatements = ifStatements;
    expression->elseStatements = elseStatements;
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
        if (expressionType == ExpressionType::ADD)        { tokenTypes.push_back(TokenType::ADD); }
        if (expressionType == ExpressionType::SUB)        { tokenTypes.push_back(TokenType::SUB); }
        if (expressionType == ExpressionType::MUL)        { tokenTypes.push_back(TokenType::MUL); }
        if (expressionType == ExpressionType::DIV)        { tokenTypes.push_back(TokenType::DIV); }
        if (expressionType == ExpressionType::IS_LESS)    { tokenTypes.push_back(TokenType::IS_LESS); }
        if (expressionType == ExpressionType::IS_LEQ)     { tokenTypes.push_back(TokenType::IS_LEQ); }
        if (expressionType == ExpressionType::IS_GREATER) { tokenTypes.push_back(TokenType::IS_GREATER); }
        if (expressionType == ExpressionType::IS_GEQ)     { tokenTypes.push_back(TokenType::IS_GEQ); }
        if (expressionType == ExpressionType::IS_EQ)      { tokenTypes.push_back(TokenType::IS_EQ); }
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
                    if (tokenType == TokenType::ADD)        { expressionType = ExpressionType::ADD; }
                    if (tokenType == TokenType::SUB)        { expressionType = ExpressionType::SUB; }
                    if (tokenType == TokenType::MUL)        { expressionType = ExpressionType::MUL; }
                    if (tokenType == TokenType::DIV)        { expressionType = ExpressionType::DIV; }
                    if (tokenType == TokenType::IS_LESS)    { expressionType = ExpressionType::IS_LESS; }
                    if (tokenType == TokenType::IS_LEQ)     { expressionType = ExpressionType::IS_LEQ; }
                    if (tokenType == TokenType::IS_GREATER) { expressionType = ExpressionType::IS_GREATER; }
                    if (tokenType == TokenType::IS_GEQ)     { expressionType = ExpressionType::IS_GEQ; }
                    if (tokenType == TokenType::IS_EQ)      { expressionType = ExpressionType::IS_EQ; }
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
            if (expression->token.tokenType != TokenType::LEFT_PAREN && expression->token.tokenType != TokenType::RIGHT_PAREN) {
                printExpr.push_back(expression);
            }
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

std::vector<std::shared_ptr<Expression>> reservedWordExpressions(const std::vector<std::shared_ptr<Expression>> expressions, const std::string& reserved) {
    std::vector<std::shared_ptr<Expression>> newExpressions;
    bool isExpr = false;
    std::vector<std::shared_ptr<Expression>> expr;
    std::shared_ptr<Expression> rootExpr;
    for (std::shared_ptr<Expression> expression : expressions) {
        if (isExpr) {
            if (expression->token.tokenType != TokenType::LEFT_PAREN && expression->token.tokenType != TokenType::RIGHT_PAREN) {
                expr.push_back(expression);
            }
        } else {
            if (expression->token.tokenType == TokenType::WORD && expression->token.payload == reserved) {
                rootExpr = expression;
                isExpr = true;
            } else {
                newExpressions.push_back(expression);
            }
        }
    }
    
    if (isExpr) {
        std::shared_ptr<Expression> newExpression;
        if (reserved == "print") { newExpression = printExpression(rootExpr, expr); }
        newExpressions.push_back(newExpression);
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
    
    newExpressions = reservedWordExpressions(newExpressions, "print");
    newExpressions = varExpressions(newExpressions);
    newExpressions = stringExpressions(newExpressions);
    newExpressions = parenExpressions(newExpressions);
    newExpressions = binaryOpExpressions(newExpressions, {ExpressionType::MUL, ExpressionType::DIV});
    newExpressions = binaryOpExpressions(newExpressions, {ExpressionType::ADD, ExpressionType::SUB});
    newExpressions = binaryOpExpressions(newExpressions, {
        ExpressionType::IS_LESS, ExpressionType::IS_LEQ, ExpressionType::IS_GREATER, ExpressionType::IS_GEQ, ExpressionType::IS_EQ
    });
    
    return newExpressions;
}

void parse(std::map<std::string, std::vector<std::vector<std::shared_ptr<Expression>>>>& funcExpressions,
           const std::vector<std::vector<std::shared_ptr<Expression>>>& expressionLines) {
    std::vector<std::vector<std::shared_ptr<Expression>>> curFuncExpressions;

    std::vector<std::vector<std::vector<std::shared_ptr<Expression>>>> curIfExpressions;
    std::vector<std::vector<std::vector<std::shared_ptr<Expression>>>> curElseExpressions;
    std::vector<std::shared_ptr<Expression>> ifExpressionRoot;
    std::vector<std::shared_ptr<Expression>> ifExpressionConditional;
    std::vector<bool> inIf;
    
    std::string funcName;
    for (const std::vector<std::shared_ptr<Expression>>& expressionLine : expressionLines) {
        if (expressionLine.size() == 0) { continue; }

        if (expressionLine[0]->token.payload == "fn") {
            funcName = expressionLine[1]->token.payload;
        } else if (expressionLine[0]->token.payload == "endfn") {
            funcExpressions[funcName] = curFuncExpressions;
            funcName = std::string();
            curFuncExpressions = std::vector<std::vector<std::shared_ptr<Expression>>>();
        } else if (expressionLine[0]->token.payload == "if") {
            inIf.push_back(true);
            
            std::vector<std::shared_ptr<Expression>> cutExpressionLine;
            for (int cutExprIdx = 1; cutExprIdx < expressionLine.size(); cutExprIdx++) {
                cutExpressionLine.push_back(expressionLine[cutExprIdx]);
            }
            
            curIfExpressions.push_back(std::vector<std::vector<std::shared_ptr<Expression>>>());
            ifExpressionConditional.push_back(parseLine(cutExpressionLine)[0]);
            ifExpressionRoot.push_back(expressionLine[0]);
        } else if (expressionLine[0]->token.payload == "endif") {
            
            std::shared_ptr<Expression> addingExpressionRoot = ifExpressionRoot.back();
            std::shared_ptr<Expression> addingExpressionConditional = ifExpressionConditional.back();
            std::vector<std::vector<std::shared_ptr<Expression>>> addingIfExpressions = curIfExpressions.back();
            std::vector<std::vector<std::shared_ptr<Expression>>> addingElseExpressions = curElseExpressions.back();
            
            ifExpressionRoot.pop_back();
            ifExpressionConditional.pop_back();
            curIfExpressions.pop_back();
            curElseExpressions.pop_back();
            
            curFuncExpressions.push_back({ifExpression(addingExpressionRoot, addingExpressionConditional, addingIfExpressions, addingElseExpressions)});
        } else if (expressionLine[0]->token.payload == "else") {
            inIf[inIf.size() - 1] = false;
            curElseExpressions.push_back(std::vector<std::vector<std::shared_ptr<Expression>>>());
        } else {
            if (curIfExpressions.size() > 0) {
                if (inIf[inIf.size() - 1]) {
                    curIfExpressions[curIfExpressions.size() - 1].push_back(parseLine(expressionLine));
                } else {
                    curElseExpressions[curIfExpressions.size() - 1].push_back(parseLine(expressionLine));
                }
            }
            
            else {
                curFuncExpressions.push_back(parseLine(expressionLine));
            }
        }
    }
}
