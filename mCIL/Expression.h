#pragma once
#include "cil-system.h"
#include "Position.h"
#include "Token.h"

enum class ExprType
{
	EXPRESSION_ERROR,
	EXPRESSION_GROUPING,
	EXPRESSION_PRIMARY,
	EXPRESSION_UNARY,
	EXPRESSION_BINARY,
	EXPRESSION_TERNARY,
	EXPRESSION_LOGICAL,
};

enum class PrimaryType
{
	PRIMARY_BOOL,
	PRIMARY_NUM,
	PRIMARY_STR,
	PRIMARY_IDENTIFIER
};

union primary_value
{
	bool bool_val;
	double num_val;
	const std::string* str_val;
	const std::string* identifier_val;
};

class Expression
{
public:
	Expression(ExprType, Position);
	virtual ~Expression();

	static Expression* make_error_expr(Position);
	static Expression* make_grouping_expr(Expression*, Position);
	static Expression* make_bool_expr(bool, Position);
	static Expression* make_num_expr(double, Position);
	static Expression* make_str_expr(const std::string&, Position);
	static Expression* make_identifier_expr(const std::string&, Position);
	static Expression* make_unary_expr(Operator, Expression*, Position);
	static Expression* make_binary_expr(Operator, Expression*, Expression*, Position);
	static Expression* make_ternary_expr(Expression*, Expression*, Expression*, Position);
	static Expression* make_logical_expr(Operator, Expression*, Expression*, Position);

	Position pos() const
	{ return this->pos_; }

	bool is_error_expr() const
	{ return this->type_ == ExprType::EXPRESSION_ERROR; }

	bool is_grouping_expr()
	{ return this->type_ == ExprType::EXPRESSION_GROUPING; }

	bool is_primary_expr()
	{ return this->type_ == ExprType::EXPRESSION_PRIMARY; }

	bool is_unary_expr()
	{ return this->type_ == ExprType::EXPRESSION_UNARY; }

	bool is_binary_expr()
	{ return this->type_ == ExprType::EXPRESSION_BINARY; }

	bool is_ternary_expr()
	{ return this->type_ == ExprType::EXPRESSION_TERNARY; }

	bool is_logical_expr()
	{ return this->type_ == ExprType::EXPRESSION_LOGICAL; }

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

class GroupingExpression : public Expression
{
public:
	GroupingExpression(Expression* expr, Position pos)
		: Expression(ExprType::EXPRESSION_GROUPING, pos), expr_(expr) {}
private:
	Expression* expr_;
};

class PrimaryExpression : public Expression
{
public:
	PrimaryExpression(PrimaryType type, primary_value val, Position pos)
		: Expression(ExprType::EXPRESSION_PRIMARY, pos), primary_type_(type), val_(val) {}

private:
	PrimaryType primary_type_;
	primary_value val_;
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

class LogicalExpression : public Expression
{
public:
	LogicalExpression(Operator op, Expression* left, Expression* right, Position pos)
		: Expression(ExprType::EXPRESSION_LOGICAL, pos), op_(op), left_(left), right_(right) {}
private:
	Operator op_;
	Expression* left_;
	Expression* right_;
};