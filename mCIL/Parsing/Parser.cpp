#include "Parser.h"

Parser::Parser(std::vector<Token>& tokens)
    : tokens_(tokens), current(0), func_level(0), loop_level(0) {}

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

void Parser::synchronize()
{
    while (true)
    {
        if (this->match_symbol(Symbol::SEMICOLON))
        {
            break;
        }
        if (this->match_keyword(Keyword::KEYWORD_PRINT) ||
            this->match_keyword(Keyword::KEYWORD_IF   ) ||
            this->match_keyword(Keyword::KEYWORD_FOR  ) ||
            this->match_keyword(Keyword::KEYWORD_WHILE) ||
            this->match_keyword(Keyword::KEYWORD_DEF  ) ||
            this->match_keyword(Keyword::KEYWORD_CLASS) ||
            this->atEnd())
        {
            this->current--;
            break;
        }
        this->current++;
    }
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

bool Parser::get_type(cilType& type)
{
    bool is_const = this->match_keyword(Keyword::KEYWORD_CONST);

    type = cilType(Type::ERROR);

    const Token token = this->peek();
    if (token.is_keyword())
    {
        switch (token.keyword())
        {
        case Keyword::KEYWORD_BOOL:
            type = cilType(Type::BOOL, is_const);
            break;
        case Keyword::KEYWORD_NUM:
            type = cilType(Type::NUM, is_const);
            break;
        case Keyword::KEYWORD_STR:
            type = cilType(Type::STR, is_const);
            break;
        case Keyword::KEYWORD_AUTO:
            type = cilType(Type::UNKNOWN, is_const);
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
        { throw CILError::error(right_paren.position(), "Expected ')'"); }
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

expr_ptr Parser::call_expr()
{
    const Token id = this->peek();
    if (this->match_identifier())
    {
        const Token l_paren = this->peek();
        if (this->match_symbol(Symbol::LEFT_PAREN))
        {
            expr_list args;
            Token r_paren = this->peek();
            while (true)
            {
                if (this->atEnd())
                { throw CILError::error(l_paren.position(), "Exptected ')'"); }
                expr_ptr arg = this->expression();
                args.push_back(arg);
                const Token comma = this->peek();
                r_paren = this->peek();
                if (!this->match_symbol(Symbol::COMMA))
                {
                    if(this->match_symbol(Symbol::RIGHT_PAREN))
                    { break; }
                    throw CILError::error(comma.position(), "Exptected ','");
                }
            }
            return Expression::make_call_expr(id, args, this->pos_from_tokens(id, r_paren));
        }
        this->current--;
    }
    return this->primary_expr();
}

expr_ptr Parser::array_access_expr()
{
    const Token id = this->peek();
    if (this->match_identifier())
    {
        const Token l_bracket = this->peek();
        if (this->match_symbol(Symbol::LEFT_BRACKET))
        {
            expr_ptr index = this->expression();
            if(!this->match_symbol(Symbol::RIGHT_BRACKET))
            { throw CILError::error(index->pos(), "Expected ']'"); }
            return Expression::make_array_access_expr(id, index, this->pos_from_tokens(id, this->peek()));
        }
        this->current--;
    }
    return this->call_expr();
}

expr_ptr Parser::unary_expr()
{
    const Token op_token = this->peek();
    if (this->match_operators(Operator::OPERATOR_BANG, Operator::OPERATOR_SUBTRACT))
    {
        expr_ptr right = this->unary_expr();
        return Expression::make_unary_expr(op_token, right);
    }
    return this->array_access_expr();
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
        expr_ptr index = Expression::make_error_expr(this->peek().position());
        int temp_current = this->current;
        if (this->match_symbol(Symbol::LEFT_BRACKET))
        {
            index = this->expression();
            if(!this->match_symbol(Symbol::RIGHT_BRACKET))
            { throw CILError::error(this->peek().position(), "Expected ']'"); }
        }
        const Token token = this->peek();
        while (this->match_operator(Operator::OPERATOR_EQUAL))
        {
            expr_ptr right = this->assignment_expr();
            return Expression::make_assignment_expr(id, index, right);
        }
        if(!index->is_error_expr())
        { this->current = temp_current; }
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
        expr = this->assignment_expr();
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

stmt_ptr Parser::break_stmt()
{
    const Token break_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_BREAK))
    {
        if (this->loop_level < 1)
        { throw CILError::error(break_keyword.position(), "'break' can only be used inside a loop body"); }
        const Token semicolon = this->peek();
        this->consume_semicolon(break_keyword.position());
        return Statement::make_break_stmt(this->pos_from_tokens(break_keyword, semicolon));
    }
    return this->block_stmt();
}

stmt_ptr Parser::return_stmt()
{
    const Token ret_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_RETURN))
    {
        if(this->func_level < 1)
        { throw CILError::error(ret_keyword.position(), "'return' can only be used inside a function body"); }
        expr_ptr expr = this->expression();
        const Token semicolon = this->peek();
        this->consume_semicolon(expr->pos());
        return Statement::make_return_stmt(expr, this->pos_from_tokens(ret_keyword, semicolon));
    }
    return this->break_stmt();
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
    return this->return_stmt();
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
        this->loop_level++;
        stmt_ptr inner = this->statement();
        this->loop_level--;
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
        this->loop_level++;
        stmt_ptr inner = this->statement();
        this->loop_level--;
        return Statement::make_for_stmt(init, cond, exec, inner, Position{ for_keyword.position(), inner->pos() });
    }
    return this->while_stmt();
}

stmt_ptr Parser::var_decl_stmt()
{
    cilType type;
    const Token type_t = this->peek();
    if (this->get_type(type))
    {
        const Token name = this->peek();
        if (!this->match_identifier())
        { throw CILError::error(name.position(), "Expected variable name"); }
        if (!this->match_operator(Operator::OPERATOR_EQUAL))
        {
            //TODO: Change this
            CILError::error(name.position(), "Variables must be initialized for now");
        }
        expr_ptr val = this->expression();
        const Token semicolon = this->peek();
        this->consume_semicolon(val->pos());
        VarInfo info
        {
            .name = name.identifier(),
            .type = type
        };
        return Statement::make_var_decl_stmt(info, val, this->pos_from_tokens(type_t, semicolon));
    }
    return this->for_stmt();
}

stmt_ptr Parser::arr_decl_stmt()
{
    cilType type;
    const Token type_t = this->peek();
    if (this->get_type(type))
    {
        if (this->match_symbol(Symbol::LEFT_BRACKET))
        {
            const Token size = this->peek();
            if(!this->match_number())
            { throw CILError::error(size.position(), "Expected size of the array"); }
            if(!this->match_symbol(Symbol::RIGHT_BRACKET))
            { throw CILError::error(size.position(), "Expected ']'"); }
            const Token name = this->peek();
            if (!this->match_identifier())
            { throw CILError::error(size.position(), "Expected name of the array"); }
            if (!this->match_operator(Operator::OPERATOR_EQUAL))
            {
                //TODO: Change this
                CILError::error(name.position(), "Arrays must be initialized for now");
            }
            if (!this->match_symbol(Symbol::LEFT_BRACE))
            { throw CILError::error(size.position(), "Expected '{'"); }
            expr_list vals{};
            while (!this->match_symbol(Symbol::RIGHT_BRACE))
            {
                if (this->atEnd())
                { throw CILError::error(this->peek().position(), "Expected '}'"); }
                expr_ptr val = this->expression();
                vals.push_back(val);
                if (!this->match_symbol(Symbol::COMMA))
                {
                    if (!this->match_symbol(Symbol::RIGHT_BRACE))
                    { throw CILError::error(this->peek().position(), "Expected ','"); }
                    break;
                }
            }
            const Token semicolon = this->peek();
            this->consume_semicolon(this->peek().position());

            ArrInfo info
            {
                .name = name.identifier(),
                .type = type,
                .size = (int)size.number_val()
            };
            return Statement::make_arr_decl_stmt(info, vals, this->pos_from_tokens(type_t, semicolon));
        }
        else
        { this->current -= (type.is_const ? 2 : 1); }
    }
    return this->var_decl_stmt();
}

stmt_ptr Parser::func_decl_stmt()
{
    const Token def_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_DEF))
    {
        const Token name = this->peek();
        if (!this->match_identifier())
        { throw CILError::error(name.position(), "Expected function name"); }
        if (!this->match_symbol(Symbol::LEFT_PAREN))
        { throw CILError::error(name.position(), "Expected '('"); }
        std::vector<VarInfo> args{};
        while (!this->match_symbol(Symbol::RIGHT_PAREN))
        {
            if(this->atEnd())
            { throw CILError::error(this->peek().position(), "Expected ')'"); }
            cilType arg_type;
            const Token arg_type_t = this->peek();
            if (!this->get_type(arg_type))
            { throw CILError::error(arg_type_t.position(), "Expected argument type"); }
            const Token arg_name = this->peek();
            if (!this->match_identifier())
            { throw CILError::error(arg_name.position(), "Expected argument name"); }
            args.push_back(VarInfo{ arg_name.identifier(), arg_type});
            if (!this->match_symbol(Symbol::COMMA))
            {
                if (!this->match_symbol(Symbol::RIGHT_PAREN))
                { throw CILError::error(this->peek().position(), "Expected ','"); }
                break;
            }
        }
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        { throw CILError::error(name.position(), "Expected ')'"); }
        cilType ret_type = cilType(Type::NONE);
        if (this->match_symbol(Symbol::ARROW))
        {
            if(!this->get_type(ret_type))
            { throw CILError::error(this->peek().position(), "Expected return type"); }
        }

        FuncInfo info
        {
            .name = name.identifier(),
            .args = args,
            .ret_type = ret_type
        };

        this->func_level++;
        stmt_ptr body = this->statement();
        this->func_level--;

        return Statement::make_func_decl_stmt(info, body, Position{ def_keyword.position(), body->pos() });
    }
    return this->arr_decl_stmt();
}

stmt_ptr Parser::statement()
{
    try
    {
        return this->func_decl_stmt();
    }
    catch (CILError& err)
    {
        this->synchronize();
        ErrorManager::cil_error(err);
        return Statement::make_error_stmt(err.range());
    }
}
