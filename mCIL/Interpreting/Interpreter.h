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

	Object run_single_expression(expr_ptr expr);

private:
	Object run_expr(expr_ptr expr);

	Object run_grouping_expr(std::shared_ptr<GroupingExpression> expr);
	Object run_primary_expr(std::shared_ptr<PrimaryExpression> expr);
	Object run_unary_expr(std::shared_ptr<UnaryExpression> expr);
	Object run_binary_expr(std::shared_ptr<BinaryExpression> expr);
	Object run_ternary_expr(std::shared_ptr<TernaryExpression> expr);
	Object run_assignment_expr(std::shared_ptr<AssignmentExpression> expr);

	stmt_list& program_;
};

