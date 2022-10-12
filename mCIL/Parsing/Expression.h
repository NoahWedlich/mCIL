#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Position.h"
#include "../Lexing/Token.h"

class Expression;
typedef std::shared_ptr<Expression> expr_ptr;
typedef std::vector<expr_ptr> expr_list;

enum class ExprType
{
	EXPRESSION_ERROR,
	EXPRESSION_GROUPING,
	EXPRESSION_PRIMARY,
	EXPRESSION_CALL,
	EXPRESSION_ACCESS,
	EXPRESSION_NEW,
	EXPRESSION_ARRAY_ACCESS,
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
	Expression(ExprType type, Position pos)
		: type_(type), pos_(pos) {}

	virtual ~Expression() {}

	static expr_ptr make_error_expr(Position);
	static expr_ptr make_grouping_expr(expr_ptr, Position);
	static expr_ptr make_bool_expr(Token);
	static expr_ptr make_num_expr(Token);
	static expr_ptr make_str_expr(Token);
	static expr_ptr make_identifier_expr(Token);
	static expr_ptr make_call_expr(Token, expr_list, Position);
	static expr_ptr make_access_expr(Token, expr_ptr, Position);
	static expr_ptr make_new_expr(Token, expr_list, Position);
	static expr_ptr make_array_access_expr(Token, expr_ptr, Position);
	static expr_ptr make_unary_expr(Token, expr_ptr);
	static expr_ptr make_binary_expr(Token, expr_ptr, expr_ptr);
	static expr_ptr make_ternary_expr(expr_ptr, expr_ptr, expr_ptr);
	static expr_ptr make_assignment_expr(expr_ptr, expr_ptr);

	Position pos() const
	{ return this->pos_; }

	const ExprType type() const
	{ return this->type_; }

	bool is_error_expr() const
	{ return this->type_ == ExprType::EXPRESSION_ERROR; }

	bool is_grouping_expr()
	{ return this->type_ == ExprType::EXPRESSION_GROUPING; }

	bool is_primary_expr()
	{ return this->type_ == ExprType::EXPRESSION_PRIMARY; }

	bool is_call_expr()
	{ return this->type_ == ExprType::EXPRESSION_CALL; }

	bool is_access_expr()
	{ return type_ == ExprType::EXPRESSION_ACCESS; }

	bool is_new_expr()
	{ return type_ == ExprType::EXPRESSION_NEW; }

	bool is_array_access_expr()
	{ return this->type_ == ExprType::EXPRESSION_ARRAY_ACCESS; }

	bool is_unary_expr()
	{ return this->type_ == ExprType::EXPRESSION_UNARY; }

	bool is_binary_expr()
	{ return this->type_ == ExprType::EXPRESSION_BINARY; }

	bool is_ternary_expr()
	{ return this->type_ == ExprType::EXPRESSION_TERNARY; }

	bool is_assignment_expr()
	{ return this->type_ == ExprType::EXPRESSION_ASSIGNMENT; }

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

	const expr_ptr expr() const
	{ return expr_; }
private:
	expr_ptr expr_;
};

class PrimaryExpression : public Expression
{
public:
	PrimaryExpression(PrimaryType type, primary_value val, Position pos)
		: Expression(ExprType::EXPRESSION_PRIMARY, pos), primary_type_(type), val_(val) {}

	const PrimaryType primary_type() const
	{ return primary_type_; }

	const primary_value val() const
	{ return val_; }
private:
	PrimaryType primary_type_;
	primary_value val_;
};

class CallExpression : public Expression
{
public:
	CallExpression(const std::string& identifier, expr_list args, Position pos)
		: Expression(ExprType::EXPRESSION_CALL, pos), identifier_(identifier), args_(args) {}

	const std::string& identifier() const
	{ return identifier_; }

	const expr_list args() const
	{ return args_; }
private:
	const std::string& identifier_;
	expr_list args_;
};

class AccessExpression : public Expression
{
public:
	AccessExpression(const std::string identifier, expr_ptr inner, Position pos)
		: Expression(ExprType::EXPRESSION_ACCESS, pos), identifier_(identifier), inner_(inner) {}

	const std::string& identifier() const
	{ return identifier_; }

	const expr_ptr inner() const
	{ return inner_; }
private:
	const std::string identifier_;
	expr_ptr inner_;
};

class NewExpression : public Expression
{
public:
	NewExpression(const std::string identifier, expr_list args, Position pos)
		: Expression(ExprType::EXPRESSION_NEW, pos), identifier_(identifier), args_(args) {}

	const std::string& identifier() const
	{ return identifier_; }

	const expr_list args() const
	{ return args_; }
private:
	const std::string identifier_;
	expr_list args_;
};

class ArrayAccessExpression : public Expression
{
public:
	ArrayAccessExpression(const std::string& identifier, expr_ptr index, Position pos)
		: Expression(ExprType::EXPRESSION_ARRAY_ACCESS, pos), identifier_(identifier), index_(index) {}

	const std::string& identifier() const
	{ return identifier_; }

	const expr_ptr index() const
	{ return index_; }
private:
	const std::string& identifier_;
	expr_ptr index_;
};

class UnaryExpression : public Expression
{
public:
	UnaryExpression(Operator op, expr_ptr expr, Position pos)
		: Expression(ExprType::EXPRESSION_UNARY, pos), op_(op), expr_(expr) {}

	const Operator op() const
	{ return op_; }

	const expr_ptr expr() const
	{ return expr_; }
private:
	Operator op_;
	expr_ptr expr_;
};

class BinaryExpression : public Expression
{
public:
	BinaryExpression(Operator op, expr_ptr left, expr_ptr right, Position pos)
		: Expression(ExprType::EXPRESSION_BINARY, pos), op_(op), left_(left), right_(right) {}

	const Operator op() const
	{ return op_; }

	const expr_ptr left() const
	{ return left_; }

	const expr_ptr right() const
	{ return right_; }
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


	const expr_ptr cond() const
	{ return cond_; }

	const expr_ptr left() const
	{ return left_; }

	const expr_ptr right() const
	{ return right_; }
private:
	expr_ptr cond_;
	expr_ptr left_;
	expr_ptr right_;
};

class AssignmentExpression : public Expression
{
public:
	AssignmentExpression(expr_ptr target, expr_ptr right, Position pos)
		: Expression(ExprType::EXPRESSION_ASSIGNMENT, pos), target_(target), expr_(right) {}

	expr_ptr target() const
	{ return target_; }

	const expr_ptr expr() const
	{ return expr_; }
private:
	expr_ptr target_;
	expr_ptr expr_;
};