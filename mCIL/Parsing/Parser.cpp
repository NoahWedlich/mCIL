#include "Parser.h"

Parser::Parser(token_list& tokens)
    : tokens_(tokens), current(0), func_level(0), loop_level(0), has_return_(false) {}

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
    if (!this->atEnd())
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
        throw CILError::error(peek()->pos(), "Expected 'number' got '$'", peek()->type());
    }
}

void Parser::expect_str()
{
    if (!match_string())
    {
        throw CILError::error(peek()->pos(), "Expected 'string' got '$'", peek()->type());
    }
}

void Parser::expect_identifier()
{
    if (!match_identifier())
    {
        throw CILError::error(peek()->pos(), "Expected 'identifier' got '$'", peek()->type());
    }
}

void Parser::expect_symbol(Symbol symbol)
{
    if (peek()->type() != TokenType::TOKEN_SYMBOL)
    {
        throw CILError::error(peek()->pos(), "Expected 'symbol' got '$'", peek()->type());
    }
    else if (!match_symbol(symbol))
    {
        throw CILError::error(peek()->pos(), "Expected '$' got '$'", symbol, peek()->symbol());
    }
}

void Parser::expect_op(Operator op)
{
    if (peek()->type() != TokenType::TOKEN_OPERATOR)
    {
        throw CILError::error(peek()->pos(), "Expected 'operator' got '$'", peek()->type());
    }
    else if (!match_operator(op))
    {
        throw CILError::error(peek()->pos(), "Expected '$' got '$'", op, peek()->op());
    }
}

void Parser::expect_keyword(Keyword keyword)
{
    if (peek()->type() != TokenType::TOKEN_KEYWORD)
    {
        throw CILError::error(peek()->pos(), "Expected 'keyword' got '$'", peek()->type());
    }
    else if (!match_keyword(keyword))
    {
        throw CILError::error(peek()->pos(), "Expected '$' got '$'", keyword, peek()->keyword());
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
        if (this->match_symbol(Symbol::LEFT_BRACE))
        {
            this->current--;
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

Position Parser::pos_from_tokens(token_ptr start, token_ptr end)
{
    return Position(start->pos().start_pos(), end->pos().end_pos());
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
    const token_ptr token = this->peek();
    if (token->is_number())
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_string()
{
    const token_ptr token = this->peek();
    if (token->is_string())
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_identifier()
{
    const token_ptr token = this->peek();
    if (token->is_identifier())
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_symbol(Symbol sym)
{
    const token_ptr token = this->peek();
    if (token->is_symbol() && token->symbol() == sym)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_operator(Operator op)
{
    const token_ptr token = this->peek();
    if (token->is_operator() && token->op() == op)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_keyword(Keyword key)
{
    const token_ptr token = this->peek();
    if (token->is_keyword() && token->keyword() == key)
    {
        this->advance();
        return true;
    }
    return false;
}

bool Parser::match_type()
{
    bool is_const = this->match_keyword(Keyword::KEYWORD_CONST);

    const token_ptr token = this->peek();
    if (token->is_identifier())
    {
        if (TypeTable::type_exists(token->identifier()))
        {
            if (is_const)
            { current--; }
            return true;
        }
        //TODO: Maybe add optimistic type checking here
        return false;
    }
    return false;
}

bool Parser::get_type(Type& type)
{
    bool is_const = this->match_keyword(Keyword::KEYWORD_CONST);

    type = Type::make("error");

    const token_ptr token = this->peek();
    if (token->is_identifier())
    {
        if (TypeTable::type_exists(token->identifier()))
        {
            type = Type::make(token->identifier(), (is_const ? TypeFlags::CONST : 0));
            advance();
            return true;
        }
        //TODO: Add optimistic type checking
        return false;
    }
    return false;
}

expr_ptr Parser::grouping_expr()
{
    const token_ptr left_paren = this->peek();
    if (this->match_symbol(Symbol::LEFT_PAREN))
    {
        expr_ptr expr = this->expression();
        const token_ptr right_paren = this->peek();
        if (!this->match_symbol(Symbol::RIGHT_PAREN))
        { throw CILError::error(right_paren->pos(), "Expected ')'"); }
        return Expression::make_grouping_expr(expr, this->pos_from_tokens(left_paren, right_paren));
    }
    throw CILError::error(left_paren->pos(), "Expected an expression");
}

expr_ptr Parser::primary_expr()
{
    const token_ptr token = this->peek();

    //TODO: Add support for 'none' keyword
    /*if (match_keyword(Keyword::KEYWORD_NONE))
    {
        return Expression::make_none_expr(token);
    }*/
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
    return this->grouping_expr();
}

expr_ptr Parser::call_expr()
{
    const token_ptr id = this->peek();
    if (this->match_identifier())
    {
        const token_ptr l_paren = this->peek();
        if (this->match_symbol(Symbol::LEFT_PAREN))
        {
            expr_list args;
            token_ptr r_paren = this->peek();
            while (!match_symbol(Symbol::RIGHT_PAREN))
            {
                if (this->atEnd())
                { throw CILError::error(l_paren->pos(), "Exptected ')'"); }
                expr_ptr arg = this->expression();
                args.push_back(arg);
                const token_ptr comma = this->peek();
                r_paren = this->peek();
                if (!this->match_symbol(Symbol::COMMA))
                {
                    if(this->match_symbol(Symbol::RIGHT_PAREN))
                    { break; }
                    throw CILError::error(comma->pos(), "Exptected ','");
                }
            }
            //TODO: Add a local symbol table
            if (!SymbolTable::global_function_exists(id->identifier()))
			{ throw CILError::error(id->pos(), "Function '" + id->identifier() + "' does not exist"); }
            
            parse_function(SymbolTable::get_global_function(id->identifier()));

            return Expression::make_call_expr(id, args, this->pos_from_tokens(id, r_paren));
        }
        this->current--;
    }
    return this->primary_expr();
}

expr_ptr Parser::access_expr()
{
    const token_ptr id = this->peek();
    if (this->match_identifier())
    {
        const token_ptr dot = this->peek();
        if (this->match_symbol(Symbol::DOT))
        {
            expr_ptr inner = call_expr();
            return Expression::make_access_expr(id, inner, Position(id->pos().range(), inner->pos().range()));
        }
        this->current--;
    }
    return this->call_expr();
}

expr_ptr Parser::new_expr()
{
    const token_ptr new_t = peek();
    if (match_keyword(Keyword::KEYWORD_NEW))
    {
        const token_ptr id = peek();
        expect_identifier();
        const token_ptr l_paren = peek();
        expect_symbol(Symbol::LEFT_PAREN);

        expr_list args;
        token_ptr r_paren = peek();
        while (!match_symbol(Symbol::RIGHT_PAREN))
        {
            if (atEnd())
            {
                throw CILError::error(l_paren->pos(), "Exptected ')'");
            }
            expr_ptr arg = expression();
            args.push_back(arg);
            const token_ptr comma = peek();
            r_paren = this->peek();
            if (!this->match_symbol(Symbol::COMMA))
            {
                if (this->match_symbol(Symbol::RIGHT_PAREN))
                { break; }
                throw CILError::error(comma->pos(), "Exptected ','");
            }
        }
        return Expression::make_new_expr(id, args, pos_from_tokens(new_t, r_paren));
    }
    return access_expr();
}

expr_ptr Parser::array_access_expr()
{
    const token_ptr id = this->peek();
    if (this->match_identifier())
    {
        const token_ptr l_bracket = this->peek();
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
    token_ptr op_token = this->peek();
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
    token_ptr token = this->peek();
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
    token_ptr token = this->peek();
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
    token_ptr token = this->peek();
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
    token_ptr token = this->peek();
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
    token_ptr token = this->peek();
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
    token_ptr token = this->peek();
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
    token_ptr token = this->peek();
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
    token_ptr token = this->peek();
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
    const token_ptr token = this->peek();
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
    const token_ptr token = this->peek();
    expr_ptr expr = Expression::make_error_expr(token->pos());
    try
    {
        expr = this->assignment_expr();
    }
    catch(CILError)
    {
        throw CILError::error(token->pos(), "Expected a statement");
    }
    const token_ptr semicolon = this->peek();
    expect_symbol(Symbol::SEMICOLON);
    return Statement::make_expr_stmt(expr, Position{ expr->pos(), semicolon->pos() });
}

stmt_ptr Parser::block_stmt()
{
    const token_ptr l_brace = this->peek();
    if (this->match_symbol(Symbol::LEFT_BRACE))
    {
        stmt_list inner{};
        token_ptr r_brace = this->peek();
        while (!this->atEnd())
        {
            inner.push_back(this->statement());
            r_brace = this->peek();
            if (this->match_symbol(Symbol::RIGHT_BRACE))
            {
                return Statement::make_block_stmt(inner, this->pos_from_tokens(l_brace, r_brace));
            }
        }
        throw CILError::error(r_brace->pos(), "Expected '}'");
    }
    return this->expr_stmt();
}

stmt_ptr Parser::break_stmt()
{
    const token_ptr break_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_BREAK))
    {
        if (this->loop_level < 1)
        { throw CILError::error(break_keyword->pos(), "'break' can only be used inside a loop body"); }
        const token_ptr semicolon = this->peek();
        expect_symbol(Symbol::SEMICOLON);
        return Statement::make_break_stmt(this->pos_from_tokens(break_keyword, semicolon));
    }
    return this->block_stmt();
}

stmt_ptr Parser::return_stmt()
{
    const token_ptr ret_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_RETURN))
    {
        if(this->func_level < 1)
        { throw CILError::error(ret_keyword->pos(), "'return' can only be used inside a function body"); }
        expr_ptr expr = this->expression();
        const token_ptr semicolon = this->peek();
        expect_symbol(Symbol::SEMICOLON);
        has_return_ = true;
        return Statement::make_return_stmt(expr, this->pos_from_tokens(ret_keyword, semicolon));
    }
    return this->break_stmt();
}

stmt_ptr Parser::print_stmt()
{
    const token_ptr print_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_PRINT))
    {
        expr_ptr expr = this->expression();
        const token_ptr semicolon = this->peek();
        expect_symbol(Symbol::SEMICOLON);
        return Statement::make_print_stmt(expr, this->pos_from_tokens(print_keyword, semicolon));
    }
    return this->return_stmt();
}

stmt_ptr Parser::elif_stmt()
{
    const token_ptr elif_keyword = peek();
    if (match_keyword(Keyword::KEYWORD_ELIF))
    {
        expect_symbol(Symbol::LEFT_PAREN);
        expr_ptr cond = expression();
        expect_symbol(Symbol::RIGHT_PAREN);
        stmt_ptr inner = statement();
        stmt_ptr next_elif = elif_stmt();
        return Statement::make_elif_stmt(cond, inner, next_elif, Position{ elif_keyword->pos(), inner->pos() });
    }
    else if (match_keyword(Keyword::KEYWORD_ELSE))
    {
        stmt_ptr inner = statement();
        return Statement::make_elif_stmt(nullptr, inner, nullptr, Position{ elif_keyword->pos(), inner->pos() });
    }
    return nullptr;
}

stmt_ptr Parser::if_stmt()
{
    const token_ptr if_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_IF))
    {
        expect_symbol(Symbol::LEFT_PAREN);
        expr_ptr cond = this->expression();
        expect_symbol(Symbol::RIGHT_PAREN);
        stmt_ptr if_branch = this->statement();
        stmt_ptr top_elif = elif_stmt();

        return Statement::make_if_stmt(cond, if_branch, top_elif, Position{ if_keyword->pos(), if_branch->pos() });
    }
    return this->print_stmt();
}

stmt_ptr Parser::while_stmt()
{
    const token_ptr while_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_WHILE))
    {
        expect_symbol(Symbol::LEFT_PAREN);
        expr_ptr cond = this->expression();
        expect_symbol(Symbol::RIGHT_PAREN);
        this->loop_level++;
        stmt_ptr inner = this->statement();
        this->loop_level--;
        return Statement::make_while_stmt(cond, inner, Position{ while_keyword->pos(), inner->pos() });
    }
    return this->if_stmt();
}

stmt_ptr Parser::for_stmt()
{
    const token_ptr for_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_FOR))
    {
        expect_symbol(Symbol::LEFT_PAREN);
        stmt_ptr init = this->statement();
        expr_ptr cond = this->expression();
        expect_symbol(Symbol::SEMICOLON);
        expr_ptr exec = this->expression();
        expect_symbol(Symbol::RIGHT_PAREN);
        this->loop_level++;
        stmt_ptr inner = this->statement();
        this->loop_level--;
        return Statement::make_for_stmt(init, cond, exec, inner, Position{ for_keyword->pos(), inner->pos() });
    }
    return this->while_stmt();
}

stmt_ptr Parser::var_decl_stmt()
{
    Type type = Type::make("error");
    const token_ptr type_t = this->peek();
    if (this->get_type(type))
    {
        const token_ptr name = this->peek();
        if (match_identifier())
        {
            expr_ptr val;
            if (match_operator(Operator::OPERATOR_EQUAL))
            { val = expression(); }
            else
            { val = Expression::make_none_expr(Token::create_invalid_token(Position{0,0})); }
            const token_ptr semicolon = this->peek();
            expect_symbol(Symbol::SEMICOLON);
            VarInfo info
            {
                .name = name->identifier(),
                .type = type
            };
            return Statement::make_var_decl_stmt(info, val, this->pos_from_tokens(type_t, semicolon));
        }
        else
        { this->current -= (type.is_const() ? 2 : 1); }
    }
    return this->for_stmt();
}

stmt_ptr Parser::arr_decl_stmt()
{
    //TODO: Make default constructor
    Type type = Type::make("error");
    const token_ptr type_t = this->peek();
    if (this->get_type(type))
    {
        if (this->match_symbol(Symbol::LEFT_BRACKET))
        {
            const token_ptr size = this->peek();
            expect_number();
            expect_symbol(Symbol::RIGHT_BRACKET);
            const token_ptr name = this->peek();
            expect_identifier();
            if (!this->match_operator(Operator::OPERATOR_EQUAL))
            {
                //TODO: Change this
                CILError::error(name->pos(), "Arrays must be initialized for now");
            }
            expect_symbol(Symbol::LEFT_BRACE);
            expr_list vals{};
            while (!this->match_symbol(Symbol::RIGHT_BRACE))
            {
                if (this->atEnd())
                { throw CILError::error(this->peek()->pos(), "Expected '}'"); }
                expr_ptr val = this->expression();
                vals.push_back(val);
                if (!this->match_symbol(Symbol::COMMA))
                {
                    if (!this->match_symbol(Symbol::RIGHT_BRACE))
                    { throw CILError::error(this->peek()->pos(), "Expected ','"); }
                    break;
                }
            }
            const token_ptr semicolon = this->peek();
            expect_symbol(Symbol::SEMICOLON);

            ArrInfo info
            {
                .name = name->identifier(),
                .type = type,
                .size = (int)size->number_val()
            };
            return Statement::make_arr_decl_stmt(info, vals, this->pos_from_tokens(type_t, semicolon));
        }
        else
        { this->current -= (type.is_const() ? 2 : 1); }
    }
    return this->var_decl_stmt();
}

stmt_ptr Parser::func_decl_stmt()
{
    const token_ptr def_keyword = this->peek();
    if (this->match_keyword(Keyword::KEYWORD_DEF))
    {
        const token_ptr name = this->peek();
        expect_identifier();
        expect_symbol(Symbol::LEFT_PAREN);
        std::vector<VarInfo> args{};
        while (!this->match_symbol(Symbol::RIGHT_PAREN))
        {
            if(this->atEnd())
            { throw CILError::error(this->peek()->pos(), "Expected ')'"); }
            Type arg_type = Type::make("error");
            const token_ptr arg_type_t = this->peek();
            if (!this->get_type(arg_type))
            { throw CILError::error(arg_type_t->pos(), "Expected argument type"); }
            const token_ptr arg_name = this->peek();
            expect_identifier();
            args.push_back(VarInfo{ arg_name->identifier(), arg_type});
            if (!this->match_symbol(Symbol::COMMA))
            {
                if (!this->match_symbol(Symbol::RIGHT_PAREN))
                { throw CILError::error(this->peek()->pos(), "Expected ','"); }
                break;
            }
        }
        Type ret_type = Type::make("none");
        if (this->match_symbol(Symbol::ARROW))
        {
            if(!this->get_type(ret_type))
            { throw CILError::error(this->peek()->pos(), "Expected return type"); }
        }

        FuncInfo info
        {
            .name = name->identifier(),
            .args = args,
            .ret_type = ret_type,
            .has_return = false
        };

        stmt_ptr body = nullptr;

        token_ptr semicolon = peek();
        if (!match_symbol(Symbol::SEMICOLON))
        {
            this->func_level++;
            stmt_ptr body = this->statement();
            this->func_level--;

            if (has_return_)
            {
                info.has_return = true;
                has_return_ = false;
            }

            return Statement::make_func_decl_stmt(info, body, Position{ def_keyword->pos(), body->pos() });
        }

        return Statement::make_func_decl_stmt(info, body, Position{ def_keyword->pos(), semicolon->pos() });
    }
    return this->arr_decl_stmt();
}

stmt_ptr Parser::class_decl_stmt()
{
    const token_ptr class_keyword = peek();
    if (match_keyword(Keyword::KEYWORD_CLASS))
    {
        const token_ptr name = peek();
        expect_identifier();
        const token_ptr l_brace = peek();
        expect_symbol(Symbol::LEFT_BRACE);

        stmt_list methods{};
        std::vector<FuncInfo> methods_info{};
        stmt_list members{};
        std::vector<VarInfo> members_info{};

        token_ptr r_brace = peek();
        while (!atEnd())
        {
            r_brace = peek();

            const token_ptr current = peek();
            if (current->is_keyword() && current->keyword() == Keyword::KEYWORD_DEF)
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
                    .name = name->identifier(),
                    .methods = methods_info,
                    .members = members_info
                };

                //TODO: Local class types shouldn't be registered in the global type table
                TypeTable::register_type(info.name, type_id("object"));

                return Statement::make_class_decl_stmt(info, methods, members, pos_from_tokens(class_keyword, r_brace));
            }
            else
            {
                throw CILError::error(current->pos(), "Only method and member-declarations are allowed inside a class-body");
            }
        }
        throw CILError::error(r_brace->pos(), "Expected '}'");
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

void Parser::parse_variable(SymbolTable::Variable& var)
{
    //TODO: Implement variable parsing
}

void Parser::parse_function(SymbolTable::Function& func)
{
    //TODO: Implement parsing of parameter-default values
    Parser p{ func.tokens };
    p.func_level++;
    stmt_ptr body = p.parse_single_stmt();
    func.body = body;
    func.parsed = true;
}

void Parser::parse_class(SymbolTable::Class& cls)
{
    //TODO: Implement class parsing
}
