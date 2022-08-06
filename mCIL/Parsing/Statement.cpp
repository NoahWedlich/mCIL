#include "Statement.h"

stmt_ptr Statement::make_error_stmt(Position pos)
{
    return stmt_ptr(new ErrorStatement(pos));
}

stmt_ptr Statement::make_print_stmt(expr_ptr expr, Position pos)
{
    return stmt_ptr(new PrintStatement(expr, pos));
}

stmt_ptr Statement::make_if_stmt(expr_ptr cond, stmt_ptr if_branch, Position pos)
{
    return stmt_ptr(new IfStatement(cond, if_branch, pos));
}

stmt_ptr Statement::make_while_stmt(expr_ptr cond, stmt_ptr inner, Position pos)
{
    return stmt_ptr(new WhileStatement(cond, inner, pos));
}

stmt_ptr Statement::make_for_stmt(stmt_ptr init, expr_ptr cond, stmt_ptr exec, stmt_ptr inner, Position pos)
{
    return stmt_ptr(new ForStatement(init, cond, exec, inner, pos));
}

stmt_ptr Statement::make_expr_stmt(expr_ptr expr, Position pos)
{
    return stmt_ptr(new ExprStatement(expr, pos));
}
