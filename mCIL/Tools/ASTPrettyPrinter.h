#pragma once
#include "../cil-system.h"
#include "../Diagnostics/TokenPrinter.h"
#include "../Parsing/Expression.h"

class ASTPrettyPrinter
{
public:
	ASTPrettyPrinter(std::ostream& os = std::cout)
		: os_(os) {}

	void print_expression(expr_ptr expr);

private:
	std::ostream& os_;

	std::string repr_expr(expr_ptr expr);

	std::string repr_grouping(std::shared_ptr<GroupingExpression> expr);
	std::string repr_primary(std::shared_ptr<PrimaryExpression> expr);
	std::string repr_unary(std::shared_ptr<UnaryExpression> expr);
	std::string repr_binary(std::shared_ptr<BinaryExpression> expr);
	std::string repr_ternary(std::shared_ptr<TernaryExpression> expr);
	std::string repr_assignment(std::shared_ptr<AssignmentExpression> expr);
};

