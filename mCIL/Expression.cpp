#include "Expression.h"

Expression::Expression(ExprType type, Position pos)
	: type_(type), pos_(pos) {}

Expression::~Expression()
{
}

Expression* Expression::make_error_expr(Position pos)
{
	return new ErrorExpression(pos);
}

Expression* Expression::make_grouping_expr(Expression* expr, Position pos)
{
	return new GroupingExpression(expr, pos);
}

Expression* Expression::make_bool_expr(bool val, Position pos)
{
	primary_value value{};
	value.bool_val = val;
	return new PrimaryExpression(PrimaryType::PRIMARY_BOOL, value, pos);
}

Expression* Expression::make_num_expr(double val, Position pos)
{
	primary_value value{};
	value.num_val = val;
	return new PrimaryExpression(PrimaryType::PRIMARY_NUM, value, pos);
}

Expression* Expression::make_str_expr(const std::string& val, Position pos)
{
	primary_value value{};
	value.str_val = &val;
	return new PrimaryExpression(PrimaryType::PRIMARY_STR, value, pos);
}

Expression* Expression::make_identifier_expr(const std::string& val, Position pos)
{
	primary_value value{};
	value.identifier_val = &val;
	return new PrimaryExpression(PrimaryType::PRIMARY_IDENTIFIER, value, pos);
}

Expression* Expression::make_unary_expr(Operator op, Expression* expr, Position pos)
{
	return new UnaryExpression(op, expr, pos);
}

Expression* Expression::make_binary_expr(Operator op, Expression* left, Expression* right, Position pos)
{
	return new BinaryExpression(op, left, right, pos);
}

Expression* Expression::make_ternary_expr(Expression* cond, Expression* left, Expression* right, Position pos)
{
	return new TernaryExpression(cond, left, right, pos);
}
