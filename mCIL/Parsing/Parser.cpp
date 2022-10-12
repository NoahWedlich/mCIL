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

void Parser::expect_number()
{
    if (!match_number())
    {
        throw CILError::error(peek().pos(), "Expected 'number' got '$'", peek().type());
    }
}

void Parser::expect_str()
{
    if (!match_string())
    {
        throw CILError::error(peek().pos(), "Expected 'string' got '$'", peek().type());
    }
}

void Parser::expect_identifier()
{
    if (!match_identifier())
    {
        throw CILError::error(peek().pos(), "Expected 'identifier' got '$'", peek().type());
    }
}

void Parser::expect_symbol(Symbol symbol)
{
    if (peek().type() != TokenType::TOKEN_SYMBOL)
    {
        throw CILError::error(peek().pos(), "Expected 'symbol' got '$'", peek().type());
    }
    else if (!match_symbol(symbol))
    {
        throw CILError::error(peek().pos(), "Expected '$' got '$'", symbol, peek().symbol());
    }
}

void Parser::expect_op(Operator op)
{
    if (peek().type() != TokenType::TOKEN_OPERATOR)
    {
        throw CILError::error(peek().pos(), "Expected 'operator' got '$'", peek().type());
    }
    else if (!match_operator(op))
    {
        throw CILError::error(peek().pos(), "Expected '$' got '$'", op, peek().op());
    }
}

void Parser::expect_keyword(Keyword keyword)
{
    if (peek().type() != TokenType::TOKEN_KEYWORD)
    {
        throw CILError::error(peek().pos(), "Expected 'keyword' got '$'", peek().type());
    }
    else if (!match_keyword(keyword))
    {
        throw CILError::error(peek().pos(), "Expected '$' got '$'", keyword, peek().keyword());
    }
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
    return Position(start.pos().start_pos(), end.pos().end_pos());
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

bool Parser::match_type()
{
    bool is_const = this->match_keyword(Keyword::KEYWORD_CONST);

    const Token token = this->peek();
    if (token.is_keyword())
    {
        switch (token.keyword())
        {
        case Keyword::KEYWORD_BOOL:
        case Keyword::KEYWORD_NUM:
        case Keyword::KEYWORD_STR:
        case Keyword::KEYWORD_AUTO:
            if(is_const)
            { current--; }
            return true;
        default:
            return false;
        }
    }
    else if (token.is_identifier())
    {
        if (is_const)
        { current--; }
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
    else if (token.is_identifier())
    {
        type = cilType(Type::OBJ, is_const);
        advance();
        return true;
    }
    return false;
}

expr_ptr Parser::grouping_expr()
{
    const Token left_paren = this->peek();
    if (this->match_symbol(Symbol::LEFT_PAREN))
    {
        expr_ptr expr = this->expression();
        const Token right_paren = this->peek();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        { throw CILError::error(right_paren.pos(), "Expected ')'"); }
        return Expression::make_grouping_expr(expr, this->pos_from_tokens(left_paren, right_paren));
    }
    throw CILError::error(left_paren.pos(), "Expected an expression");
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
            while (!match_symbol(Symbol::RIGHT_PAREN))
            {
                if (this->atEnd())
                { throw CILError::error(l_paren.pos(), "Exptected ')'"); }
                expr_ptr arg = this->expression();
                args.push_back(arg);
                const Token comma = this->peek();
                r_paren = this->peek();
                if (!this->match_symbol(Symbol::COMMA))
                {
                    if(this->match_symbol(Symbol::RIGHT_PAREN))
                    { break; }
                    throw CILError::error(comma.pos(), "Exptected ','");
                }
            }
            return Expression::make_call_expr(id, args, this->pos_from_tokens(id, r_paren));
        }
        this->current--;
    }
    return this->primary_expr();
}

expr_ptr Parser::access_expr()
{
    const Token id = this->peek();
    if (this->match_identifier())
    {
        const Token dot = this->peek();
        if (this->match_symbol(Symbol::DOT))
        {
            expr_ptr inner = call_expr();
            return Expression::make_access_expr(id, inner, Position(id.pos().range(), inner->pos().range()));
        }
        this->current--;
    }
    return this->call_expr();
}

expr_ptr Parser::new_expr()
{
    const Token new_t = peek();
    if (match_keyword(Keyword::KEYWORD_NEW))
    {
        const Token id = peek();
        expect_identifier();
        const Token l_paren = peek();
        expect_symbol(Symbol::LEFT_PAREN);

        expr_list args;
        Token r_paren = peek();
        while (!match_symbol(Symbol::RIGHT_PAREN))
        {
            if (atEnd())
            {
                throw CILError::error(l_paren.pos(), "Exptected ')'");
            }
            expr_ptr arg = expression();
            args.push_back(arg);
            const Token comma = peek();
            r_paren = this->peek();
            if (!this->match_symbol(Symbol::COMMA))
            {
                if (this->match_symbol(Symbol::RIGHT_PAREN))
                { break; }
                throw CILError::error(comma.pos(), "Exptected ','");
            }
        }
        return Expression::make_new_expr(id, args, pos_from_tokens(new_t, r_paren));
    }
    return access_expr();
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
            expect_symbol(Symbol::RIGHT_BRACKET);
            return Expression::make_array_access_expr(id, index, this->pos_from_tokens(id, this->peek()));
        }
        this->current--;
    }
    return this->new_expr();
}

expr_ptr Parser::unary_expr()
{
    Token op_token = this->peek();
    if (this->match_operators(Operator::OPERATOR_BANG, Operator::OPERATOR_SUBTRACT) ||
        this->match_operator(Operator::OPERATOR_BITWISE_NOT))
    {
        expr_ptr right = this->unary_expr();
        return Expression::make_unary_expr(op_token, right);
    }
    expr_ptr inner = array_access_expr();
    op_token = this->peek();
    if (match_operators(Operator::OPERATOR_INCREMENT, Operator::OPERATOR_DECREMENT))
    {
        return Expression::make_unary_expr(op_token, inner);
    }
    return inner;
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

expr_ptr Parser::bitshift_expr()
{
    expr_ptr left = this->sum_expr();
    Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_LEFT_BITSHIFT, Operator::OPERATOR_RIGHT_BITSHIFT))
    {
        expr_ptr right = this->sum_expr();
        left = Expression::make_binary_expr(token, left, right);
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::comparison_expr()
{
    expr_ptr left = this->bitshift_expr();
    Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_GREATER, Operator::OPERATOR_GREATER_EQUAL) ||
           this->match_operators(Operator::OPERATOR_LESS   , Operator::OPERATOR_LESS_EQUAL   ))
    {
        expr_ptr right = this->bitshift_expr();
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

expr_ptr Parser::bitwise_expr()
{
    expr_ptr left = this->equality_expr();
    Token token = this->peek();
    while (this->match_operators(Operator::OPERATOR_BITWISE_AND, Operator::OPERATOR_BITWISE_XOR) ||
           this->match_operator(Operator::OPERATOR_BITWISE_OR))
    {
        expr_ptr right = this->equality_expr();
        left = Expression::make_binary_expr(token, left, right);
        token = this->peek();
    }
    return left;
}

expr_ptr Parser::logical_and_expr()
{
    expr_ptr left = this->bitwise_expr();
    Token token = this->peek();
    while (this->match_operator(Operator::OPERATOR_AND))
    {
        expr_ptr right = this->bitwise_expr();
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
    //TODO: Implement call.identifier
    expr_ptr target = this->ternary_expr();
    while (this->match_operator(Operator::OPERATOR_EQUAL))
    {
        expr_ptr value = this->assignment_expr();
        target = Expression::make_assignment_expr(target, value);
    }
    return target;
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
    expr_ptr expr = Expression::make_error_expr(token.pos());
    try
    {
        expr = this->assignment_expr();
    }
    catch(CILError)
    {
        throw CILError::error(token.pos(), "Expected a statement");
    }
    const Token semicolon = this->peek();
    expect_symbol(Symbol::SEMICOLON);
    return Statement::make_expr_stmt(expr, Position{ expr->pos(), semicolon.pos() });
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
        throw CILError::error(r_brace.pos(), "Expected '}'");
    }
    return this->expr_stmt();
}

stmt_ptr Parser::break_stmt()
{
    const Token break_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_BREAK))
    {
        if (this->loop_level < 1)
        { throw CILError::error(break_keyword.pos(), "'break' can only be used inside a loop body"); }
        const Token semicolon = this->peek();
        expect_symbol(Symbol::SEMICOLON);
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
        { throw CILError::error(ret_keyword.pos(), "'return' can only be used inside a function body"); }
        expr_ptr expr = this->expression();
        const Token semicolon = this->peek();
        expect_symbol(Symbol::SEMICOLON);
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
        expect_symbol(Symbol::SEMICOLON);
        return Statement::make_print_stmt(expr, this->pos_from_tokens(print_keyword, semicolon));
    }
    return this->return_stmt();
}

stmt_ptr Parser::if_stmt()
{
    const Token if_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_IF))
    {
        expect_symbol(Symbol::LEFT_PAREN);
        expr_ptr cond = this->expression();
        expect_symbol(Symbol::RIGHT_PAREN);
        stmt_ptr if_branch = this->statement();
        stmt_ptr else_branch = nullptr;

        if (match_keyword(Keyword::KEYWORD_ELSE))
        {
            else_branch = statement();
        }

        return Statement::make_if_stmt(cond, if_branch, else_branch, Position{ if_keyword.pos(), if_branch->pos() });
    }
    return this->print_stmt();
}

stmt_ptr Parser::while_stmt()
{
    const Token while_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_WHILE))
    {
        expect_symbol(Symbol::LEFT_PAREN);
        expr_ptr cond = this->expression();
        expect_symbol(Symbol::RIGHT_PAREN);
        this->loop_level++;
        stmt_ptr inner = this->statement();
        this->loop_level--;
        return Statement::make_while_stmt(cond, inner, Position{ while_keyword.pos(), inner->pos() });
    }
    return this->if_stmt();
}

stmt_ptr Parser::for_stmt()
{
    const Token for_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_FOR))
    {
        expect_symbol(Symbol::RIGHT_PAREN);
        stmt_ptr init = this->statement();
        expr_ptr cond = this->expression();
        expect_symbol(Symbol::SEMICOLON);
        expr_ptr exec = this->expression();
        expect_symbol(Symbol::RIGHT_PAREN);
        this->loop_level++;
        stmt_ptr inner = this->statement();
        this->loop_level--;
        return Statement::make_for_stmt(init, cond, exec, inner, Position{ for_keyword.pos(), inner->pos() });
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
        if (match_identifier())
        {
            if (!this->match_operator(Operator::OPERATOR_EQUAL))
            {
                //TODO: Change this
                CILError::error(name.pos(), "Variables must be initialized for now");
            }
            expr_ptr val = this->expression();
            const Token semicolon = this->peek();
            expect_symbol(Symbol::SEMICOLON);
            VarInfo info
            {
                .name = name.identifier(),
                .type = type
            };
            return Statement::make_var_decl_stmt(info, val, this->pos_from_tokens(type_t, semicolon));
        }
        else
        { this->current -= (type.is_const ? 2 : 1); }
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
            expect_number();
            expect_symbol(Symbol::RIGHT_BRACKET);
            const Token name = this->peek();
            expect_identifier();
            if (!this->match_operator(Operator::OPERATOR_EQUAL))
            {
                //TODO: Change this
                CILError::error(name.pos(), "Arrays must be initialized for now");
            }
            expect_symbol(Symbol::LEFT_BRACE);
            expr_list vals{};
            while (!this->match_symbol(Symbol::RIGHT_BRACE))
            {
                if (this->atEnd())
                { throw CILError::error(this->peek().pos(), "Expected '}'"); }
                expr_ptr val = this->expression();
                vals.push_back(val);
                if (!this->match_symbol(Symbol::COMMA))
                {
                    if (!this->match_symbol(Symbol::RIGHT_BRACE))
                    { throw CILError::error(this->peek().pos(), "Expected ','"); }
                    break;
                }
            }
            const Token semicolon = this->peek();
            expect_symbol(Symbol::SEMICOLON);

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
        expect_identifier();
        expect_symbol(Symbol::LEFT_PAREN);
        std::vector<VarInfo> args{};
        while (!this->match_symbol(Symbol::RIGHT_PAREN))
        {
            if(this->atEnd())
            { throw CILError::error(this->peek().pos(), "Expected ')'"); }
            cilType arg_type;
            const Token arg_type_t = this->peek();
            if (!this->get_type(arg_type))
            { throw CILError::error(arg_type_t.pos(), "Expected argument type"); }
            const Token arg_name = this->peek();
            expect_identifier();
            args.push_back(VarInfo{ arg_name.identifier(), arg_type});
            if (!this->match_symbol(Symbol::COMMA))
            {
                if (!this->match_symbol(Symbol::RIGHT_PAREN))
                { throw CILError::error(this->peek().pos(), "Expected ','"); }
                break;
            }
        }
        cilType ret_type = cilType(Type::NONE);
        if (this->match_symbol(Symbol::ARROW))
        {
            if(!this->get_type(ret_type))
            { throw CILError::error(this->peek().pos(), "Expected return type"); }
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

        return Statement::make_func_decl_stmt(info, body, Position{ def_keyword.pos(), body->pos() });
    }
    return this->arr_decl_stmt();
}

stmt_ptr Parser::class_decl_stmt()
{
    const Token class_keyword = peek();
    if (match_keyword(Keyword::KEYWORD_CLASS))
    {
        const Token name = peek();
        expect_identifier();
        const Token l_brace = peek();
        expect_symbol(Symbol::LEFT_BRACE);

        stmt_list methods{};
        std::vector<FuncInfo> methods_info{};
        stmt_list members{};
        std::vector<VarInfo> members_info{};

        Token r_brace = peek();
        while (!atEnd())
        {
            r_brace = peek();

            const Token current = peek();
            if (current.is_keyword() && current.keyword() == Keyword::KEYWORD_DEF)
            {
                stmt_ptr method = func_decl_stmt();
                std::shared_ptr<FuncDeclStatement> cast_method = std::dynamic_pointer_cast<FuncDeclStatement, Statement>(method);
                FuncInfo method_info = cast_method->info();
                methods.push_back(method);
                methods_info.push_back(method_info);
            }
            else if (match_type())
            {
                stmt_ptr member = var_decl_stmt();
                std::shared_ptr<VarDeclStatement> cast_member = std::dynamic_pointer_cast<VarDeclStatement, Statement>(member);
                VarInfo member_info = cast_member->info();
                members.push_back(member);
                members_info.push_back(member_info);
            }
            else if (match_symbol(Symbol::RIGHT_BRACE))
            {
                ClassInfo info
                {
                    .name = name.identifier(),
                    .methods = methods_info,
                    .members = members_info
                };

                return Statement::make_class_decl_stmt(info, methods, members, pos_from_tokens(class_keyword, r_brace));
            }
            else
            {
                throw CILError::error(current.pos(), "Only method and member-declarations are allowed inside a class-body");
            }
        }
        throw CILError::error(r_brace.pos(), "Expected '}'");
    }
    return func_decl_stmt();
}

stmt_ptr Parser::statement()
{
    try
    {
        return this->class_decl_stmt();
    }
    catch (CILError& err)
    {
        this->synchronize();
        ErrorManager::cil_error(err);
        return Statement::make_error_stmt(err.range());
    }
}
