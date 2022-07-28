#include "Expression.h"

expr_ptr Expression::make_error_expr(Position pos)
{
	return expr_ptr(new ErrorExpression(pos));
}

expr_ptr Expression::make_grouping_expr(expr_ptr expr, Position pos)
{
	return expr_ptr(new GroupingExpression(expr, pos));
}

expr_ptr Expression::make_bool_expr(bool val, Position pos)
{
	primary_value value{};
	value.bool_val = val;
	return expr_ptr(new PrimaryExpression(PrimaryType::PRIMARY_BOOL, value, pos));
}

expr_ptr Expression::make_num_expr(double val, Position pos)
{
	primary_value value{};
	value.num_val = val;
	return expr_ptr(new PrimaryExpression(PrimaryType::PRIMARY_NUM, value, pos));
}

expr_ptr Expression::make_str_expr(const std::string& val, Position pos)
{
	primary_value value{};
	value.str_val = &val;
	return expr_ptr(new PrimaryExpression(PrimaryType::PRIMARY_STR, value, pos));
}

expr_ptr Expression::make_identifier_expr(const std::string& val, Position pos)
{
	primary_value value{};
	value.identifier_val = &val;
	return expr_ptr(new PrimaryExpression(PrimaryType::PRIMARY_IDENTIFIER, value, pos));
}

expr_ptr Expression::make_unary_expr(Operator op, expr_ptr expr, Position pos)
{
	return expr_ptr(new UnaryExpression(op, expr, pos));
}

expr_ptr Expression::make_binary_expr(Operator op, expr_ptr left, expr_ptr right, Position pos)
{
	return expr_ptr(new BinaryExpression(op, left, right, pos));
}

expr_ptr Expression::make_ternary_expr(expr_ptr cond, expr_ptr left, expr_ptr right, Position pos)
{
	return expr_ptr(new TernaryExpression(cond, left, right, pos));
}

expr_ptr Expression::make_assignment_expr(Token token, expr_ptr right, Position pos)
{
	return expr_ptr(new AssignmentExpression(token.identifier(), right, pos));
}
