#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Position.h"
#include "../Lexing/Token.h"

class Expression;
typedef std::shared_ptr<Expression> expr_ptr;
typedef std::vector<expr_ptr> program_t;

enum class ExprType
{
	EXPRESSION_ERROR,
	EXPRESSION_GROUPING,
	EXPRESSION_PRIMARY,
	EXPRESSION_UNARY,
	EXPRESSION_BINARY,
	EXPRESSION_TERNARY,
	EXPRESSION_ASSIGNMENT
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

	static expr_ptr make_error_expr(Position);
	static expr_ptr make_grouping_expr(expr_ptr, Position);
	static expr_ptr make_bool_expr(bool, Position);
	static expr_ptr make_num_expr(double, Position);
	static expr_ptr make_str_expr(const std::string&, Position);
	static expr_ptr make_identifier_expr(const std::string&, Position);
	static expr_ptr make_unary_expr(Operator, expr_ptr, Position);
	static expr_ptr make_binary_expr(Operator, expr_ptr, expr_ptr, Position);
	static expr_ptr make_ternary_expr(expr_ptr, expr_ptr, expr_ptr, Position);
	static expr_ptr make_assignment_expr(Token, expr_ptr, Position);

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

	bool is_assignment_expr()
	{ return this->type_ == ExprType::EXPRESSION_ASSIGNMENT; }

	friend class ASTDebugPrinter;
	friend class ASTPrettyPrinter;

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
	GroupingExpression(expr_ptr expr, Position pos)
		: Expression(ExprType::EXPRESSION_GROUPING, pos), expr_(expr) {}

	friend class ASTDebugPrinter;
	friend class ASTPrettyPrinter;
private:
	expr_ptr expr_;
};

class PrimaryExpression : public Expression
{
public:
	PrimaryExpression(PrimaryType type, primary_value val, Position pos)
		: Expression(ExprType::EXPRESSION_PRIMARY, pos), primary_type_(type), val_(val) {}

	friend class ASTDebugPrinter;
	friend class ASTPrettyPrinter;
private:
	PrimaryType primary_type_;
	primary_value val_;
};

class UnaryExpression : public Expression
{
public:
	UnaryExpression(Operator op, expr_ptr expr, Position pos)
		: Expression(ExprType::EXPRESSION_UNARY, pos), op_(op), expr_(expr) {}

	friend class ASTDebugPrinter;
	friend class ASTPrettyPrinter;
private:
	Operator op_;
	expr_ptr expr_;
};

class BinaryExpression : public Expression
{
public:
	BinaryExpression(Operator op, expr_ptr left, expr_ptr right, Position pos)
		: Expression(ExprType::EXPRESSION_BINARY, pos), op_(op), left_(left), right_(right) {}

	friend class ASTDebugPrinter;
	friend class ASTPrettyPrinter;
private:
	Operator op_;
	expr_ptr left_;
	expr_ptr right_;
};

class TernaryExpression : public Expression
{
public:
	TernaryExpression(expr_ptr cond, expr_ptr left, expr_ptr right, Position pos)
		: Expression(ExprType::EXPRESSION_TERNARY, pos), cond_(cond), left_(left), right_(right) {}

	friend class ASTDebugPrinter;
	friend class ASTPrettyPrinter;
private:
	expr_ptr cond_;
	expr_ptr left_;
	expr_ptr right_;
};

class AssignmentExpression : public Expression
{
public:
	AssignmentExpression(const std::string& identifier, expr_ptr right, Position pos)
		: Expression(ExprType::EXPRESSION_ASSIGNMENT, pos), identifier_(identifier), right_(right) {}

	friend class ASTDebugPrinter;
	friend class ASTPrettyPrinter;
private:
	const std::string& identifier_;
	expr_ptr right_;
};