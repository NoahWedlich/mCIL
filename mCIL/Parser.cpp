#include "Parser.h"

//TODO: Error reporting

Parser::Parser(std::vector<Token>& tokens)
    : tokens_(tokens), current(0) {}

std::vector<Expression>* Parser::parse()
{
    std::vector<Expression>* expressions = new std::vector<Expression>();
    while (!this->atEnd())
    {
        expressions->push_back(*(this->expression()));
    }
    return expressions;
}

bool Parser::match_number()
{
    const Token token = this->peek();
    if (token.type() == TokenType::TOKEN_NUMBER)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_string()
{
    const Token token = this->peek();
    if (token.type() == TokenType::TOKEN_STRING)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_identifier()
{
    const Token token = this->peek();
    if (token.type() == TokenType::TOKEN_IDENTIFIER)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_symbol(Symbol sym)
{
    const Token token = this->peek();
    if (token.type() == TokenType::TOKEN_SYMBOL && token.symbol() == sym)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_operator(Operator op)
{
    const Token token = this->peek();
    if (token.type() == TokenType::TOKEN_OPERATOR && token.op() == op)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_keyword(Keyword key)
{
    const Token token = this->peek();
    if (token.type() == TokenType::TOKEN_KEYWORD && token.keyword() == key)
    {
        this->advance();
        return true;
    }
    return false;
}

Expression* Parser::grouping_expr()
{
    if (this->match_symbol(Symbol::LEFT_PAREN))
    {
        Expression* expr = this->expression();
        const Token rightParen = this->advance();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        {
            //TODO: Handle this error
            throw std::invalid_argument("Expected ')'");
        }
        //TODO: Refactor position
        return Expression::make_grouping_expr(expr, Position{ 0, 0, 0 });
    }
    //TODO: Handle this error
    throw std::invalid_argument("Expected Expression");
}

Expression* Parser::primary_expr()
{
    const Token token = this->peek();

    //TODO: Refactor positions
    if (this->match_number())
    {
        return Expression::make_num_expr(token.number_val(), Position{ token.position()});
    }
    else if (this->match_string())
    {
        return Expression::make_str_expr(token.string_val(), Position{ token.position() });
    }
    else if (this->match_identifier())
    {
        return Expression::make_identifier_expr(token.identifier(), Position{ token.position() });
    }
    else if (this->match_keyword(Keyword::KEYWORD_TRUE))
    {
        return Expression::make_bool_expr(true, Position{ token.position() });
    }
    else if (this->match_keyword(Keyword::KEYWORD_FALSE))
    {
        return Expression::make_bool_expr(false, Position{ token.position() });
    }
    //TODO: Implement 'this' and 'none'
    return this->grouping_expr();
}

//TODO: Add the 'call' rule

Expression* Parser::unary_expr()
{
    const Token token = this->peek();

    if (this->match_operators(Operator::OPERATOR_BANG, Operator::OPERATOR_SUBTRACT))
    {
        //TODO: Refactor position
        return Expression::make_unary_expr(token.op(), this->unary_expr(), token.position());
    }
    //TODO: Change to 'call'
    return this->primary_expr();
}

Expression* Parser::factor_expr()
{
    Expression* left = this->unary_expr();
    const Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_MULTIPLY, Operator::OPERATOR_DIVIDE))
    {
        Expression* right = this->unary_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
    }
    return left;
}

Expression* Parser::sum_expr()
{
    Expression* left = this->factor_expr();
    const Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_ADD, Operator::OPERATOR_SUBTRACT))
    {
        Expression* right = this->factor_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
    }
    return left;
}

Expression* Parser::comparison_expr()
{
    Expression* left = this->sum_expr();
    const Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_GREATER, Operator::OPERATOR_GREATER_EQUAL) ||
           this->match_operators(Operator::OPERATOR_LESS   , Operator::OPERATOR_LESS_EQUAL   ))
    {
        Expression* right = this->sum_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
    }
    return left;
}

Expression* Parser::equality_expr()
{
    Expression* left = this->comparison_expr();
    const Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_EQUAL_EQUAL, Operator::OPERATOR_NOT_EQUAL))
    {
        Expression* right = this->comparison_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
    }
    return left;
}

Expression* Parser::logical_and_expr()
{
    Expression* left = this->equality_expr();
    const Token token = this->peek();
    while (this->match_operator(Operator::OPERATOR_AND))
    {
        Expression* right = this->equality_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
    }
    return left;
}

Expression* Parser::logical_or_expr()
{
    Expression* left = this->logical_and_expr();
    const Token token = this->peek();
    while (this->match_operator(Operator::OPERATOR_OR))
    {
        Expression* right = this->logical_and_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
    }
    return left;
}

Expression* Parser::ternary_expr()
{
    Expression* expr = this->logical_and_expr();
    const Token token = this->peek();
    if (this->match_symbol(Symbol::QUERY))
    {
        Expression* left = this->logical_or_expr();
        if (this->match_symbol(Symbol::COLON))
        {
            Expression* right = this->logical_or_expr();
            return Expression::make_ternary_expr(expr, left, right, Position{ token.position() });
        }
        else
        {
            //TODO: Handle this error
            throw std::invalid_argument("Expected ':' after '?'");
        }
    }
    return expr;
}

Expression* Parser::assignment_expr()
{
    const Token id = this->peek();
    //TODO: Implement call.identifier
    if (this->match_identifier())
    {
        const Token token = this->peek();
        if (this->match_operator(Operator::OPERATOR_EQUAL))
        {
            Expression* right = this->assignment_expr();
            return Expression::make_assignment_expr(id, right, Position{ token.position() });
        }
    }
    return this->ternary_expr();
}

Expression* Parser::expression()
{
    return this->assignment_expr();
}
