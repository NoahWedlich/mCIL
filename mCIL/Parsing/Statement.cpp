#include "Statement.h"

stmt_ptr Statement::make_error_stmt(Position pos)
{
    return stmt_ptr(new ErrorStatement(pos));
}

stmt_ptr Statement::make_block_stmt(stmt_list inner, Position pos)
{
    return stmt_ptr(new BlockStatement(inner, pos));
}

stmt_ptr Statement::make_break_stmt(Position pos)
{
    return stmt_ptr(new BreakStatement(pos));
}

stmt_ptr Statement::make_return_stmt(expr_ptr expr, Position pos)
{
    return stmt_ptr(new ReturnStatement(expr, pos));
}

stmt_ptr Statement::make_print_stmt(expr_ptr expr, Position pos)
{
    return stmt_ptr(new PrintStatement(expr, pos));
}

stmt_ptr Statement::make_if_stmt(expr_ptr cond, stmt_ptr if_branch, stmt_ptr top_elif, Position pos)
{
    return stmt_ptr(new IfStatement(cond, if_branch, top_elif, pos));
}

stmt_ptr Statement::make_elif_stmt(expr_ptr cond, stmt_ptr inner, stmt_ptr next_elif, Position pos)
{
    return stmt_ptr(new ElifStatement(cond, inner, next_elif, pos));
}

stmt_ptr Statement::make_while_stmt(expr_ptr cond, stmt_ptr inner, Position pos)
{
    return stmt_ptr(new WhileStatement(cond, inner, pos));
}

stmt_ptr Statement::make_for_stmt(stmt_ptr init, expr_ptr cond, expr_ptr exec, stmt_ptr inner, Position pos)
{
    return stmt_ptr(new ForStatement(init, cond, exec, inner, pos));
}

stmt_ptr Statement::make_var_decl_stmt(VarInfo info, expr_ptr val, Position pos)
{
    return stmt_ptr(new VarDeclStatement(info, val, pos));
}

stmt_ptr Statement::make_arr_decl_stmt(ArrInfo info, expr_list vals, Position pos)
{
    return stmt_ptr(new ArrDeclStatement(info, vals, pos));
}

stmt_ptr Statement::make_func_decl_stmt(FuncInfo info, stmt_ptr body, Position pos)
{
    return stmt_ptr(new FuncDeclStatement(info, body, pos));
}

stmt_ptr Statement::make_class_decl_stmt(ClassInfo info, stmt_list methods, stmt_list members, Position pos)
{
    return stmt_ptr(new ClassDeclStatement(info, methods, members, pos));
}

stmt_ptr Statement::make_expr_stmt(expr_ptr expr, Position pos)
{
    return stmt_ptr(new ExprStatement(expr, pos));
}
