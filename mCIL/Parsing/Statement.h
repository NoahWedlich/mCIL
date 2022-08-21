#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Position.h"
#include "../Interpreting/Object.h"
#include "../Interpreting/Variable.h"
#include "../Interpreting/Function.h"
#include "Expression.h"

class Statement;
typedef std::shared_ptr<Statement> stmt_ptr;
typedef std::vector<stmt_ptr> stmt_list;

enum class StmtType
{
	STATEMENT_ERROR,
	STATEMENT_BLOCK,
	STATEMENT_BREAK,
	STATEMENT_RETURN,
	STATEMENT_PRINT,
	STATEMENT_IF,
	STATEMENT_WHILE,
	STATEMENT_FOR,
	STATEMENT_VAR_DECL,
	STATEMENT_FUNC_DECL,
	STATEMENT_EXPR
};

class Statement
{
public:
	Statement(StmtType type, Position pos)
		: type_(type), pos_(pos) {}

	virtual ~Statement() {}

	static stmt_ptr make_error_stmt(Position pos);
	static stmt_ptr make_block_stmt(stmt_list inner, Position pos);
	static stmt_ptr make_break_stmt(Position pos);
	static stmt_ptr make_return_stmt(expr_ptr expr, Position pos);
	static stmt_ptr make_print_stmt(expr_ptr expr, Position pos);
	//TODO: make elif and else
	static stmt_ptr make_if_stmt(expr_ptr cond, stmt_ptr if_branch, Position pos);
	static stmt_ptr make_while_stmt(expr_ptr cond, stmt_ptr inner, Position pos);
	static stmt_ptr make_for_stmt(stmt_ptr init, expr_ptr cond, expr_ptr exec, stmt_ptr inner, Position pos);
	static stmt_ptr make_var_decl_stmt(VarInfo info, expr_ptr val, Position pos);
	static stmt_ptr make_func_decl_stmt(FuncInfo info, stmt_ptr body, Position pos);
	static stmt_ptr make_expr_stmt(expr_ptr expr, Position pos);

	Position pos() const
	{ return this->pos_; }

	const StmtType type() const
	{ return this->type_; }

	bool is_error_stmt() const
	{ return this->type_ == StmtType::STATEMENT_ERROR; }

	bool is_block_stmt() const
	{ return this->type_ == StmtType::STATEMENT_BLOCK; }

	bool is_break_stmt() const
	{ return this->type_ == StmtType::STATEMENT_BREAK; }

	bool is_return_stmt() const
	{ return this->type_ == StmtType::STATEMENT_RETURN; }

	bool is_print_stmt() const
	{ return this->type_ == StmtType::STATEMENT_PRINT; }

	bool is_if_stmt() const
	{ return this->type_ == StmtType::STATEMENT_IF; }

	bool is_while_stmt() const
	{ return this->type_ == StmtType::STATEMENT_WHILE; }

	bool is_for_stmt() const
	{ return this->type_ == StmtType::STATEMENT_FOR; }

	bool is_var_decl() const
	{ return this->type_ == StmtType::STATEMENT_VAR_DECL; }

	bool is_func_decl() const
	{ return this->type_ == StmtType::STATEMENT_FUNC_DECL; }

	bool is_expr_stmt() const
	{ return this->type_ == StmtType::STATEMENT_EXPR; }

private:
	StmtType type_;
	Position pos_;
};

class ErrorStatement : public Statement
{
public:
	ErrorStatement(Position pos)
		: Statement(StmtType::STATEMENT_ERROR, pos) {}
};

class BlockStatement : public Statement
{
public:
	BlockStatement(stmt_list inner, Position pos)
		: Statement(StmtType::STATEMENT_BLOCK, pos), inner_(inner) {}

	const stmt_list inner() const
	{ return inner_; }
private:
	stmt_list inner_;
};

class BreakStatement : public Statement
{
public:
	BreakStatement(Position pos)
		: Statement(StmtType::STATEMENT_BREAK, pos) {}
};

class ReturnStatement : public Statement
{
public:
	ReturnStatement(expr_ptr expr, Position pos)
		: Statement(StmtType::STATEMENT_RETURN, pos), expr_(expr) {}

	const expr_ptr expr() const
	{ return this->expr_; }
private:
	expr_ptr expr_;
};

class PrintStatement : public Statement
{
public:
	PrintStatement(expr_ptr expr, Position pos)
		: Statement(StmtType::STATEMENT_PRINT, pos), expr_(expr) {}

	const expr_ptr expr() const
	{ return expr_; }
private:
	expr_ptr expr_;
};

class IfStatement : public Statement
{
public:
	IfStatement(expr_ptr cond, stmt_ptr if_branch, Position pos)
		: Statement(StmtType::STATEMENT_IF, pos), cond_(cond), if_branch_(if_branch) {}

	const expr_ptr cond() const
	{ return cond_; }

	const stmt_ptr if_branch() const
	{ return if_branch_; }
private:
	expr_ptr cond_;
	stmt_ptr if_branch_;
};

class WhileStatement : public Statement
{
public:
	WhileStatement(expr_ptr cond, stmt_ptr inner, Position pos)
		: Statement(StmtType::STATEMENT_WHILE, pos), cond_(cond), inner_(inner) {}

	const expr_ptr cond() const
	{ return cond_; }

	const stmt_ptr inner() const
	{ return inner_; }
private:
	expr_ptr cond_;
	stmt_ptr inner_;
};

class ForStatement : public Statement
{
public:
	ForStatement(stmt_ptr init, expr_ptr cond, expr_ptr exec, stmt_ptr inner, Position pos)
		: Statement(StmtType::STATEMENT_FOR, pos), init_(init), cond_(cond), exec_(exec), inner_(inner) {}

	const stmt_ptr init() const
	{ return init_; }

	const expr_ptr cond() const
	{ return cond_; }

	const expr_ptr exec() const
	{ return exec_; }

	const stmt_ptr inner() const
	{ return inner_; }
private:
	stmt_ptr init_;
	expr_ptr cond_;
	expr_ptr exec_;
	stmt_ptr inner_;
};

class VarDeclStatement : public Statement
{
public:
	VarDeclStatement(VarInfo info, expr_ptr val, Position pos)
		: Statement(StmtType::STATEMENT_VAR_DECL, pos), info_(info), val_(val) {}

	const VarInfo info() const
	{ return this->info_; }

	const expr_ptr val() const
	{ return this->val_; }
private:
	VarInfo info_;
	expr_ptr val_;
};

class FuncDeclStatement : public Statement
{
public:
	FuncDeclStatement(FuncInfo info, stmt_ptr body, Position pos)
		: Statement(StmtType::STATEMENT_FUNC_DECL, pos), info_(info), body_(body) {}

	const FuncInfo info() const
	{ return this->info_; }

	const stmt_ptr body() const
	{ return this->body_; }
private:
	FuncInfo info_;
	stmt_ptr body_;
};

class ExprStatement : public Statement
{
public:
	ExprStatement(expr_ptr expr, Position pos)
		: Statement(StmtType::STATEMENT_EXPR, pos), expr_(expr) {}

	const expr_ptr expr() const
	{ return expr_; }
private:
	expr_ptr expr_;
};