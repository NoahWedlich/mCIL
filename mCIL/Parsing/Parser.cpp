#include "Parser.h"

//TODO: Error reporting

Parser::Parser(std::vector<Token>& tokens)
    : tokens_(tokens), current(0) {}

program_t& Parser::parse()
{
    program_t* program = new program_t();
    while (!this->atEnd())
    {
        program->emplace_back(this->expression());
    }
    return *program;
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

expr_ptr Parser::grouping_expr()
{
    if (this->match_symbol(Symbol::LEFT_PAREN))
    {
        expr_ptr expr = this->expression();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        {
            //TODO: Handle this error
            throw std::invalid_argument("Expected ')'");
        }
        //TODO: Refactor position
        return Expression::make_grouping_expr(expr, Position{0, 0, 0});
    }
    //TODO: Handle this error
    throw std::invalid_argument("Expected Expression");
}

expr_ptr Parser::primary_expr()
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

expr_ptr Parser::unary_expr()
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

expr_ptr Parser::factor_expr()
{
    expr_ptr left = this->unary_expr();
    Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_MULTIPLY, Operator::OPERATOR_DIVIDE))
    {
        expr_ptr right = this->unary_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::sum_expr()
{
    expr_ptr left = this->factor_expr();
    Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_ADD, Operator::OPERATOR_SUBTRACT))
    {
        expr_ptr right = this->factor_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::comparison_expr()
{
    expr_ptr left = this->sum_expr();
    Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_GREATER, Operator::OPERATOR_GREATER_EQUAL) ||
           this->match_operators(Operator::OPERATOR_LESS   , Operator::OPERATOR_LESS_EQUAL   ))
    {
        expr_ptr right = this->sum_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::equality_expr()
{
    expr_ptr left = this->comparison_expr();
    Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_EQUAL_EQUAL, Operator::OPERATOR_NOT_EQUAL))
    {
        expr_ptr right = this->comparison_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::logical_and_expr()
{
    expr_ptr left = this->equality_expr();
    Token token = this->peek();
    while (this->match_operator(Operator::OPERATOR_AND))
    {
        expr_ptr right = this->equality_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::logical_or_expr()
{
    expr_ptr left = this->logical_and_expr();
    Token token = this->peek();
    while (this->match_operator(Operator::OPERATOR_OR))
    {
        expr_ptr right = this->logical_and_expr();
        //TODO: Refactor position
        left = Expression::make_binary_expr(token.op(), left, right, token.position());
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::ternary_expr()
{
    expr_ptr expr = this->logical_and_expr();
    const Token token = this->peek();
    if (this->match_symbol(Symbol::QUERY))
    {
        expr_ptr left = this->logical_or_expr();
        if (this->match_symbol(Symbol::COLON))
        {
            expr_ptr right = this->logical_or_expr();
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

expr_ptr Parser::assignment_expr()
{
    const Token id = this->peek();
    //TODO: Implement call.identifier
    if (this->match_identifier())
    {
        const Token token = this->peek();
        //TODO: Make assignment repeatable
        if (this->match_operator(Operator::OPERATOR_EQUAL))
        {
            expr_ptr right = this->assignment_expr();
            return Expression::make_assignment_expr(id, right, Position{ token.position() });
        }
    }
    return this->ternary_expr();
}

expr_ptr Parser::expression()
{
    return this->assignment_expr();
}
