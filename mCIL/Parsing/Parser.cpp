#include "Parser.h"

Parser::Parser(std::vector<Token>& tokens)
    : tokens_(tokens), current(0) {}

stmt_list& Parser::parse()
{
    stmt_list* program = new stmt_list();
    while (!this->atEnd())
    {
        program->emplace_back(this->statement());
    }
    return *program;
}

bool Parser::match_number()
{
    const Token token = this->peek();
    if (token.is_number())
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_string()
{
    const Token token = this->peek();
    if (token.is_string())
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_identifier()
{
    const Token token = this->peek();
    if (token.is_identifier())
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_symbol(Symbol sym)
{
    const Token token = this->peek();
    if (token.is_symbol() && token.symbol() == sym)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_operator(Operator op)
{
    const Token token = this->peek();
    if (token.is_operator() && token.op() == op)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_keyword(Keyword key)
{
    const Token token = this->peek();
    if (token.is_keyword() && token.keyword() == key)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::get_type(ObjType& type, bool& is_const)
{
    is_const = this->match_keyword(Keyword::KEYWORD_CONST);

    type = ObjType::ERROR;

    const Token token = this->peek();
    if (token.is_keyword())
    {
        switch (token.keyword())
        {
        case Keyword::KEYWORD_BOOL:
            type = ObjType::BOOL;
            break;
        case Keyword::KEYWORD_NUM:
            type = ObjType::NUM;
            break;
        case Keyword::KEYWORD_STR:
            type = ObjType::STR;
            break;
        case Keyword::KEYWORD_AUTO:
            type = ObjType::UNKNOWN;
            break;
        default:
            return false;
        }
        this->advance();
        return true;
    }
    return false;
}

void Parser::consume_semicolon(stmt_ptr last)
{
    const Token token = this->peek();
    if (!this->match_symbol(Symbol::SEMICOLON))
    {
        throw ParserError("Expected a semicolon", *last);
    }
}

void Parser::consume_semicolon(expr_ptr last)
{
    const Token token = this->peek();
    if (!this->match_symbol(Symbol::SEMICOLON))
    {
        throw ParserError("Expected a semicolon", *last);
    }
}

expr_ptr Parser::grouping_expr()
{
    const Token token = this->peek();
    if (this->match_symbol(Symbol::LEFT_PAREN))
    {
        expr_ptr expr = this->expression();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        {
            throw ParserError("Expected ')'", *expr);
        }
        //TODO: Refactor position
        return Expression::make_grouping_expr(expr, token.position());
    }
    throw ParserError("Expected Expression", token);
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
    expr_ptr expr = this->logical_or_expr();
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
            throw ParserError("Expected ':' after '?'", *left);
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
        this->current--;
    }
    return this->ternary_expr();
}

expr_ptr Parser::expression()
{
    try
    {
        return this->assignment_expr();
    }
    catch (ParserError err)
    {
        ErrorManager::cil_parser_error(err);
        return Expression::make_error_expr(this->peek().position());
    }
}

stmt_ptr Parser::expr_stmt()
{
    const Token token = this->peek();
    expr_ptr expr = Expression::make_error_expr(token.position());
    try
    {
        expr = this->expression();
    }
    catch(ParserError)
    {
        throw ParserError("Expected statement", token);
    }
    this->consume_semicolon(expr);
    return Statement::make_expr_stmt(expr, this->peek().position());
}

stmt_ptr Parser::block_stmt()
{
    const Token l_brace = this->peek();
    if (this->match_symbol(Symbol::LEFT_BRACE))
    {
        stmt_list inner{};
        while (!this->atEnd())
        {
            inner.push_back(this->statement());
            if (this->match_symbol(Symbol::RIGHT_BRACE))
            {
                return Statement::make_block_stmt(inner, this->peek().position());
            }
        }
        throw ParserError("Expected '}'", l_brace);
    }
    return this->expr_stmt();
}

stmt_ptr Parser::print_stmt()
{
    if (this->match_keyword(Keyword::KEYWORD_PRINT))
    {
        expr_ptr expr = this->expression();
        this->consume_semicolon(expr);
        return Statement::make_print_stmt(expr, this->peek().position());
    }
    return this->expr_stmt();
}

stmt_ptr Parser::if_stmt()
{
    const Token if_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_IF))
    {
        if (!this->match_symbol(Symbol::LEFT_PAREN))
        { throw ParserError("Expected '('", if_keyword); }
        expr_ptr cond = this->expression();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        { throw ParserError("Expected ')'", *cond); }
        stmt_ptr if_branch = this->statement();
        return Statement::make_if_stmt(cond, if_branch, this->peek().position());
    }
    return this->print_stmt();
}

stmt_ptr Parser::while_stmt()
{
    const Token while_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_WHILE))
    {
        if (!this->match_symbol(Symbol::LEFT_PAREN))
        { throw ParserError("Expected '('", while_keyword); }
        expr_ptr cond = this->expression();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        { throw ParserError("Expected ')'", *cond); }
        stmt_ptr inner = this->statement();
        return Statement::make_while_stmt(cond, inner, this->peek().position());
    }
    return this->if_stmt();
}

stmt_ptr Parser::for_stmt()
{
    const Token for_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_FOR))
    {
        if (!this->match_symbol(Symbol::LEFT_PAREN))
        { throw ParserError("Expected '('", for_keyword); }
        stmt_ptr init = this->statement();
        expr_ptr cond = this->expression();
        this->consume_semicolon(cond);
        stmt_ptr exec = this->statement();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        {  throw ParserError("Expected ')'", *exec); }
        stmt_ptr inner = this->statement();
        return Statement::make_for_stmt(init, cond, exec, inner, this->peek().position());
    }
    return this->while_stmt();
}

stmt_ptr Parser::var_decl_stmt()
{
    bool is_const;
    ObjType type;
    const Token type_t = this->peek();
    if (this->get_type(type, is_const))
    {
        const Token name = this->peek();
        if (!this->match_identifier())
        {
            throw ParserError("Expected a variable name", type_t);
        }
        if (!this->match_operator(Operator::OPERATOR_EQUAL))
        {
            throw ParserError("Variables must be initialized for now", name);
        }
        expr_ptr val = this->expression();
        this->consume_semicolon(val);
        return Statement::make_var_decl_stmt(is_const, type, name.identifier(), val, this->peek().position());
    }
    return this->while_stmt();
}

stmt_ptr Parser::statement()
{
    try
    {
        return this->var_decl_stmt();
    }
    catch (ParserError err)
    {
        ErrorManager::cil_parser_error(err);
        return Statement::make_error_stmt(this->peek().position());
    }
}

//TODO: Synchronize after error
