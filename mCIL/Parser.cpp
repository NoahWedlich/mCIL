#include "Parser.h"

//TODO: Error reporting

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
        this->advance();
        Expression* expr = this->expression();
        const Token rightParen = this->advance();
        if (!rightParen.is_symbol() || rightParen.symbol() != Symbol::RIGHT_PAREN)
        {
            //TODO: Handle this error
            throw std::invalid_argument("Expected ')'");
        }
        //TODO: Refactor position
        return Expression::make_grouping_expr(expr, Position{ token.position() });
    }
    //TODO: Handle this error
    throw std::invalid_argument("Expected Expression");
}

Expression* Parser::primary_expr()
{
    const Token token = this->peek();
    //TODO: Make tokens revertable or refactor advance/peek
    switch (token.type())
    {
    case TokenType::TOKEN_NUMBER:
        return Expression::make_num_expr(token.number_val(), Position{ token.position() });
    case TokenType::TOKEN_STRING:
        return Expression::make_str_expr(token.string_val(), Position{ token.position() });
    case TokenType::TOKEN_IDENTIFIER:
        return Expression::make_identifier_expr(token.identifier(), Position{ token.position() });
    case TokenType::TOKEN_KEYWORD:
        switch (token.keyword())
        {
        case Keyword::KEYWORD_TRUE:
            return Expression::make_bool_expr(true, Position{ token.position() });
        case Keyword::KEYWORD_FALSE:
            return Expression::make_bool_expr(false, Position{ token.position() });
        //TODO: Implement 'this' and 'none'
        }
    default:
        return this->grouping_expr();
    }
}

//TODO: Add the 'call' rule

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
