#include "Parser.h"

Parser::Parser(std::vector<Token>& tokens)
    : tokens_(tokens), current(0) {}

std::vector<Expression>& Parser::parse()
{
    std::vector<Expression> expressions{};
    while (!this->atEnd())
    {
        expressions.push_back(*(this->expression()));
    }
    return expressions;
}

Expression* Parser::grouping_expr()
{
    const Token token = this->peek();
    if (token.is_symbol() && token.symbol() == Symbol::LEFT_PAREN)
    {
        Expression* expr = this->expression();
        const Token rightParen = this->advance();
        if (!rightParen.is_symbol() || rightParen.symbol() != Symbol::RIGHT_PAREN)
        {
            throw std::invalid_argument("Expected ')'");
        }
    }

}

Expression* Parser::primary_expr()
{
    return nullptr;
}

Expression* Parser::unary_expr()
{
    return nullptr;
}

Expression* Parser::binary_expr()
{
    return nullptr;
}

Expression* Parser::ternary_expr()
{
    return nullptr;
}

Expression* Parser::logical_expr()
{
    return nullptr;
}

Expression* Parser::expression()
{
    return nullptr;
}
