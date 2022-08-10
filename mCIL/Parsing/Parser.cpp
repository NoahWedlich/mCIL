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

expr_ptr Parser::parse_single_expr()
{
    expr_ptr expr = this->expression();
    if (!this->peek().is_EOF())
    {
        return Expression::make_error_expr(Position(0, 0));
    }
    return expr;
}

stmt_ptr Parser::parse_single_stmt()
{
    return this->statement();
}

void Parser::reset()
{
    this->current = 0;
}

Position Parser::pos_from_tokens(Token start, Token end)
{
    return Position(start.position().start_pos(), end.position().end_pos());
}

Position Parser::pos_from_exprs(expr_ptr start, expr_ptr end)
{
    return Position(start->pos().start_pos(), end->pos().end_pos());
}

Position Parser::pos_from_stmts(stmt_ptr start, stmt_ptr end)
{
    return Position(start->pos().start_pos(), end->pos().end_pos());
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

void Parser::consume_semicolon(Position pos)
{
    const Token token = this->peek();
    if (!this->match_symbol(Symbol::SEMICOLON))
    {
        throw CILError::error(pos, "Expected a semicolon");
    }
}

expr_ptr Parser::grouping_expr()
{
    const Token left_paren = this->peek();
    if (this->match_symbol(Symbol::LEFT_PAREN))
    {
        expr_ptr expr = this->expression();
        const Token right_paren = this->peek();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        {
            throw CILError::error(right_paren.position(), "Expected ')'");
        }
        return Expression::make_grouping_expr(expr, this->pos_from_tokens(left_paren, right_paren));
    }
    throw CILError::error(left_paren.position(), "Expected an expression");
}

expr_ptr Parser::primary_expr()
{
    const Token token = this->peek();

    if (this->match_number())
    {
        return Expression::make_num_expr(token);
    }
    else if (this->match_string())
    {
        return Expression::make_str_expr(token);
    }
    else if (this->match_identifier())
    {
        return Expression::make_identifier_expr(token);
    }
    else if (this->match_keyword(Keyword::KEYWORD_TRUE))
    {
        return Expression::make_bool_expr(token);
    }
    else if (this->match_keyword(Keyword::KEYWORD_FALSE))
    {
        return Expression::make_bool_expr(token);
    }
    //TODO: Implement 'this' and 'none'
    return this->grouping_expr();
}

//TODO: Add the 'call' rule

expr_ptr Parser::unary_expr()
{
    const Token op_token = this->peek();
    if (this->match_operators(Operator::OPERATOR_BANG, Operator::OPERATOR_SUBTRACT))
    {
        expr_ptr right = this->unary_expr();
        return Expression::make_unary_expr(op_token, right);
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
        left = Expression::make_binary_expr(token, left, right);
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
        left = Expression::make_binary_expr(token, left, right);
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
        left = Expression::make_binary_expr(token, left, right);
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
        left = Expression::make_binary_expr(token, left, right);
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
        left = Expression::make_binary_expr(token, left, right);
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
        left = Expression::make_binary_expr(token, left, right);
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::ternary_expr()
{
    expr_ptr cond = this->logical_or_expr();
    const Token token = this->peek();
    if (this->match_symbol(Symbol::QUERY))
    {
        expr_ptr left = this->logical_or_expr();
        if (this->match_symbol(Symbol::COLON))
        {
            expr_ptr right = this->logical_or_expr();
            return Expression::make_ternary_expr(cond, left, right);
        }
        else
        {
            throw CILError::error(left->pos(), "Expected ':' after '?'");
        }
    }
    return cond;
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
            return Expression::make_assignment_expr(id, right);
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
    catch (CILError& err)
    {
        ErrorManager::cil_error(err);
        return Expression::make_error_expr(err.range());
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
    catch(CILError)
    {
        throw CILError::error(token.position(), "Expected a statement");
    }
    const Token semicolon = this->peek();
    this->consume_semicolon(expr->pos());
    return Statement::make_expr_stmt(expr, Position{ expr->pos(), semicolon.position() });
}

stmt_ptr Parser::block_stmt()
{
    const Token l_brace = this->peek();
    if (this->match_symbol(Symbol::LEFT_BRACE))
    {
        stmt_list inner{};
        Token r_brace = this->peek();
        while (!this->atEnd())
        {
            inner.push_back(this->statement());
            r_brace = this->peek();
            if (this->match_symbol(Symbol::RIGHT_BRACE))
            {
                return Statement::make_block_stmt(inner, this->pos_from_tokens(l_brace, r_brace));
            }
        }
        throw CILError::error(r_brace.position(), "Expected '}'");
    }
    return this->expr_stmt();
}

stmt_ptr Parser::print_stmt()
{
    const Token print_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_PRINT))
    {
        expr_ptr expr = this->expression();
        const Token semicolon = this->peek();
        this->consume_semicolon(expr->pos());
        return Statement::make_print_stmt(expr, this->pos_from_tokens(print_keyword, semicolon));
    }
    return this->block_stmt();
}

stmt_ptr Parser::if_stmt()
{
    const Token if_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_IF))
    {
        if (!this->match_symbol(Symbol::LEFT_PAREN))
            //TODO: Extract into function
        { throw CILError::error(if_keyword.position(), "Expected '('"); }
        expr_ptr cond = this->expression();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        { throw CILError::error(cond->pos(), "Expected ')'"); }
        stmt_ptr if_branch = this->statement();
        return Statement::make_if_stmt(cond, if_branch, Position{ if_keyword.position(), if_branch->pos() });
    }
    return this->print_stmt();
}

stmt_ptr Parser::while_stmt()
{
    const Token while_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_WHILE))
    {
        if (!this->match_symbol(Symbol::LEFT_PAREN))
        { throw CILError::error(while_keyword.position(), "Expected '('"); }
        expr_ptr cond = this->expression();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        { throw CILError::error(cond->pos(), "Expected ')'"); }
        stmt_ptr inner = this->statement();
        return Statement::make_while_stmt(cond, inner, Position{ while_keyword.position(), inner->pos() });
    }
    return this->if_stmt();
}

stmt_ptr Parser::for_stmt()
{
    const Token for_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_FOR))
    {
        if (!this->match_symbol(Symbol::LEFT_PAREN))
        { throw CILError::error(for_keyword.position(), "Expected '('"); }
        stmt_ptr init = this->statement();
        expr_ptr cond = this->expression();
        this->consume_semicolon(cond->pos());
        expr_ptr exec = this->expression();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        { throw CILError::error(exec->pos(), "Expected ')'"); }
        stmt_ptr inner = this->statement();
        return Statement::make_for_stmt(init, cond, exec, inner, Position{ for_keyword.position(), inner->pos() });
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
            throw CILError::error(name.position(), "Expected variable name");
        }
        if (!this->match_operator(Operator::OPERATOR_EQUAL))
        {
            //TODO: Change this
            CILError::error(name.position(), "Variables must be initialized for now");
        }
        expr_ptr val = this->expression();
        const Token semicolon = this->peek();
        this->consume_semicolon(val->pos());
        return Statement::make_var_decl_stmt(is_const, type, name.identifier(), val, this->pos_from_tokens(type_t, semicolon));
    }
    return this->for_stmt();
}

stmt_ptr Parser::statement()
{
    try
    {
        return this->var_decl_stmt();
    }
    catch (CILError& err)
    {
        ErrorManager::cil_error(err);
        return Statement::make_error_stmt(err.range());
    }
}

//TODO: Synchronize after error
