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

Expression* Expression::make_none_expr(Position pos)
{
	return new NoneExpression(pos);
}

Expression* Expression::make_bool_expr(bool val, Position pos)
{
	return new BoolExpression(val, pos);
}

Expression* Expression::make_num_expr(double val, Position pos)
{
	return new NumExpression(val, pos);
}

Expression* Expression::make_str_expr(const std::string& val, Position pos)
{
	return new StrExpression(val, pos);
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
