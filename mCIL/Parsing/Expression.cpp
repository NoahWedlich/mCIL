#include "Expression.h"

expr_ptr Expression::make_error_expr(Position pos)
{
	return expr_ptr(new ErrorExpression(pos));
}

expr_ptr Expression::make_grouping_expr(expr_ptr expr, Position pos)
{
	return expr_ptr(new GroupingExpression(expr, pos));
}

expr_ptr Expression::make_bool_expr(Token token)
{
	primary_value value{};
	value.bool_val = (token.keyword() == Keyword::KEYWORD_TRUE);
	return expr_ptr(new PrimaryExpression(PrimaryType::PRIMARY_BOOL, value, token.position()));
}

expr_ptr Expression::make_num_expr(Token token)
{
	primary_value value{};
	value.num_val = token.number_val();
	return expr_ptr(new PrimaryExpression(PrimaryType::PRIMARY_NUM, value, token.position()));
}

expr_ptr Expression::make_str_expr(Token token)
{
	primary_value value{};
	value.str_val = &token.string_val();
	return expr_ptr(new PrimaryExpression(PrimaryType::PRIMARY_STR, value, token.position()));
}

expr_ptr Expression::make_identifier_expr(Token token)
{
	primary_value value{};
	value.identifier_val = &token.identifier();
	return expr_ptr(new PrimaryExpression(PrimaryType::PRIMARY_IDENTIFIER, value, token.position()));
}

expr_ptr Expression::make_call_expr(Token token, expr_list args, Position pos)
{
	return expr_ptr(new CallExpression(token.identifier(), args, pos));
}

expr_ptr Expression::make_array_access_expr(Token name, expr_ptr index, Position pos)
{
	return expr_ptr(new ArrayAccessExpression(name.identifier(), index, pos));
}

expr_ptr Expression::make_unary_expr(Token token, expr_ptr expr)
{
	Position pos{ token.position(), expr->pos() };
	return expr_ptr(new UnaryExpression(token.op(), expr, pos));
}

expr_ptr Expression::make_binary_expr(Token token, expr_ptr left, expr_ptr right)
{
	Position pos{ left->pos(), right->pos()};
	return expr_ptr(new BinaryExpression(token.op(), left, right, pos));
}

expr_ptr Expression::make_ternary_expr(expr_ptr cond, expr_ptr left, expr_ptr right)
{
	Position pos{ cond->pos(), right->pos()};
	return expr_ptr(new TernaryExpression(cond, left, right, pos));
}

expr_ptr Expression::make_assignment_expr(Token token, expr_ptr index, expr_ptr right)
{
	Position pos{ token.position(), right->pos() };
	return expr_ptr(new AssignmentExpression(token.identifier(), index, right, pos));
}
