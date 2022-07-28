#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Position.h"
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
	//TODO: make break stmt
	//TODO: make return stmt
	static stmt_ptr make_print_stmt(expr_ptr expr, Position pos);
	//TODO: make elif and else
	static stmt_ptr make_if_stmt(expr_ptr cond, stmt_ptr if_branch, Position pos);
	static stmt_ptr make_while_stmt(expr_ptr cond, stmt_ptr inner, Position pos);
	static stmt_ptr make_for_stmt(stmt_ptr init, expr_ptr cond, stmt_ptr exec, stmt_ptr inner, Position pos);
	static stmt_ptr make_expr_stmt(expr_ptr expr, Position pos);

	Position pos() const
	{ return this->pos_; }

	const StmtType type() const
	{ return this->type_; }

	bool is_error_stmt() const
	{ return this->type_ == StmtType::STATEMENT_ERROR; }

	bool is_block_stmt() const
	{ return this->type_ == StmtType::STATEMENT_BLOCK; }

	bool is_print_stmt() const
	{ return this->type_ == StmtType::STATEMENT_PRINT; }

	bool is_if_stmt() const
	{ return this->type_ == StmtType::STATEMENT_IF; }

	bool is_while_stmt() const
	{ return this->type_ == StmtType::STATEMENT_WHILE; }

	bool is_for_stmt() const
	{ return this->type_ == StmtType::STATEMENT_FOR; }

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

class BlockStatment : public Statement
{
public:
	BlockStatment(stmt_list inner, Position pos)
		: Statement(StmtType::STATEMENT_BLOCK, pos), inner_(inner) {}

	const stmt_list inner() const
	{ return inner_; }
private:
	stmt_list inner_;
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

	const stmt_ptr if_branch() const
	{ return inner_; }
private:
	expr_ptr cond_;
	stmt_ptr inner_;
};

class ForStatement : public Statement
{
public:
	ForStatement(stmt_ptr init, expr_ptr cond, stmt_ptr exec, stmt_ptr inner, Position pos)
		: Statement(StmtType::STATEMENT_FOR, pos), init_(init), cond_(cond), exec_(exec), inner_(inner) {}

	const stmt_ptr init() const
	{ return init_; }

	const expr_ptr cond() const
	{ return cond_; }

	const stmt_ptr exec() const
	{ return exec_; }

	const stmt_ptr if_branch() const
	{ return inner_; }
private:
	stmt_ptr init_;
	expr_ptr cond_;
	stmt_ptr exec_;
	stmt_ptr inner_;
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