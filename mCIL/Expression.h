#pragma once
#include "cil-system.h"
#include "Position.h"
#include "Token.h"

enum class ExprType
{
	EXPRESSION_ERROR,
	EXPRESSION_NONE,
	EXPRESSION_BOOL,
	EXPRESSION_NUM,
	EXPRESSION_STR,
	EXPRESSION_UNARY,
	EXPRESSION_BINARY,
	EXPRESSION_TERNARY
};

class Expression
{
public:
	Expression(ExprType, Position);
	virtual ~Expression();

	static Expression* make_error_expr(Position);
	static Expression* make_none_expr(Position);
	static Expression* make_bool_expr(bool, Position);
	static Expression* make_num_expr(double, Position);
	static Expression* make_str_expr(const std::string&, Position);
	static Expression* make_unary_expr(Operator, Expression*, Position);
	static Expression* make_binary_expr(Operator, Expression*, Expression*, Position);
	static Expression* make_ternary_expr(Expression*, Expression*, Expression*, Position);

	Position pos() const
	{ return this->pos_; }

	bool is_error_expr() const
	{ return this->type_ == ExprType::EXPRESSION_ERROR; }

	bool is_none_expr()
	{ return this->type_ == ExprType::EXPRESSION_NONE; }

	bool is_bool_expr()
	{ return this->type_ == ExprType::EXPRESSION_BOOL; }

	bool is_num_expr()
	{ return this->type_ == ExprType::EXPRESSION_NUM; }

	bool is_str_expr()
	{ return this->type_ == ExprType::EXPRESSION_STR; }

	bool is_unary_expr()
	{ return this->type_ == ExprType::EXPRESSION_UNARY; }

	bool is_binary_expr()
	{ return this->type_ == ExprType::EXPRESSION_BINARY; }

	bool is_ternary_expr()
	{ return this->type_ == ExprType::EXPRESSION_TERNARY; }

private:
	ExprType type_;
	Position pos_;
};

class ErrorExpression : public Expression
{
public:
	ErrorExpression(Position pos)
		: Expression(ExprType::EXPRESSION_ERROR, pos) {}
};

class NoneExpression : public Expression
{
public:
	NoneExpression(Position pos)
		: Expression(ExprType::EXPRESSION_NONE, pos) {}
};

class BoolExpression : public Expression
{
public:
	BoolExpression(bool val, Position pos)
		: Expression(ExprType::EXPRESSION_BOOL, pos), val_(val) {}

private:
	bool val_;
};

class NumExpression : public Expression
{
public:
	NumExpression(double val, Position pos)
		: Expression(ExprType::EXPRESSION_NUM, pos), val_(val) {}

private:
	double val_;
};

class StrExpression : public Expression
{
public:
	StrExpression(const std::string& val, Position pos)
		: Expression(ExprType::EXPRESSION_STR, pos), val_(val) {}

private:
	const std::string val_;
};

class UnaryExpression : public Expression
{
public:
	UnaryExpression(Operator op, Expression* expr, Position pos)
		: Expression(ExprType::EXPRESSION_UNARY, pos), op_(op), expr_(expr) {}

private:
	Operator op_;
	Expression* expr_;
};

class BinaryExpression : public Expression
{
public:
	BinaryExpression(Operator op, Expression* left, Expression* right, Position pos)
		: Expression(ExprType::EXPRESSION_BINARY, pos), op_(op), left_(left), right_(right) {}

private:
	Operator op_;
	Expression* left_;
	Expression* right_;
};

class TernaryExpression : public Expression
{
public:
	TernaryExpression(Expression* cond, Expression* left, Expression* right, Position pos)
		: Expression(ExprType::EXPRESSION_TERNARY, pos), cond_(cond), left_(left), right_(right) {}

private:
	Expression* cond_;
	Expression* left_;
	Expression* right_;
};