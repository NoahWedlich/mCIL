#pragma once
#include "../cil-system.h"
#include "TokenPrinter.h"
#include "../Parsing/Expression.h"

class ASTDebugPrinter
{
public:
	ASTDebugPrinter(std::ostream& os = std::cout)
		: os_(os) {}

	void print_expression(Expression* expr);

private:
	std::ostream& os_;

	std::string repr_expr(Expression* expr);

	std::string repr_grouping(GroupingExpression* expr);
	std::string repr_primary(PrimaryExpression* expr);
	std::string repr_unary(UnaryExpression* expr);
	std::string repr_binary(BinaryExpression* expr);
	std::string repr_ternary(TernaryExpression* expr);
	std::string repr_assignment(AssignmentExpression* expr);
};

