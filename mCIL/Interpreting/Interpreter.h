#pragma once
#include "../cil-system.h"
#include "Object.h"
#include "../Parsing/Expression.h"
#include "../Parsing/Statement.h"
#include "../Diagnostics/Errors.h"
#include "../Diagnostics/Diagnostics.h"

class Interpreter
{
public:
	Interpreter(stmt_list& program);

	void run();

	void run_single_statement(stmt_ptr stmt);
	Object run_single_expression(expr_ptr expr);

private:
	void run_stmt(stmt_ptr stmt);

	void run_block_stmt(std::shared_ptr<BlockStatment> stmt);
	void run_print_stmt(std::shared_ptr<PrintStatement> stmt);
	void run_if_stmt(std::shared_ptr<IfStatement> stmt);
	void run_while_stmt(std::shared_ptr<WhileStatement> stmt);
	void run_for_stmt(std::shared_ptr<ForStatement> stmt);
	void run_expr_stmt(std::shared_ptr<ExprStatement> stmt);

	Object run_expr(expr_ptr expr);

	Object run_grouping_expr(std::shared_ptr<GroupingExpression> expr);
	Object run_primary_expr(std::shared_ptr<PrimaryExpression> expr);
	Object run_unary_expr(std::shared_ptr<UnaryExpression> expr);
	Object run_binary_expr(std::shared_ptr<BinaryExpression> expr);
	Object run_ternary_expr(std::shared_ptr<TernaryExpression> expr);
	Object run_assignment_expr(std::shared_ptr<AssignmentExpression> expr);

	stmt_list& program_;
};

