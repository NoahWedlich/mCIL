#include "ASTPrettyPrinter.h"

void ASTPrettyPrinter::print_expression(expr_ptr expr)
{
	this->os_ << this->repr_expr(expr) << std::endl;
}

std::string ASTPrettyPrinter::repr_expr(expr_ptr expr)
{
	switch (expr->type_)
	{
	case ExprType::EXPRESSION_ERROR:
		return "ERROR";
	case ExprType::EXPRESSION_GROUPING:
		return this->repr_grouping(std::dynamic_pointer_cast<GroupingExpression, Expression>(expr));
	case ExprType::EXPRESSION_PRIMARY:
		return this->repr_primary(std::dynamic_pointer_cast<PrimaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_UNARY:
		return this->repr_unary(std::dynamic_pointer_cast<UnaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_BINARY:
		return this->repr_binary(std::dynamic_pointer_cast<BinaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_TERNARY:
		return this->repr_ternary(std::dynamic_pointer_cast<TernaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_ASSIGNMENT:
		return this->repr_assignment(std::dynamic_pointer_cast<AssignmentExpression, Expression>(expr));
	default:
		return "INVALID_EXPRESSION";
	}
}

std::string ASTPrettyPrinter::repr_grouping(std::shared_ptr<GroupingExpression> expr)
{
	return "( " + this->repr_expr(expr->expr_) + " )";
}

std::string ASTPrettyPrinter::repr_primary(std::shared_ptr<PrimaryExpression> expr)
{
	switch (expr->primary_type_)
	{
	case PrimaryType::PRIMARY_BOOL:
		return expr->val_.bool_val ? "True" : "False";
	case PrimaryType::PRIMARY_NUM:
		return std::to_string(expr->val_.num_val);
	case PrimaryType::PRIMARY_STR:
		return *expr->val_.str_val;
	case PrimaryType::PRIMARY_IDENTIFIER:
		return *expr->val_.identifier_val;
	default:
		return "INVALID_PRIMARY";
	}
}

std::string ASTPrettyPrinter::repr_unary(std::shared_ptr<UnaryExpression> expr)
{
	return TokenPrinter::print_operator(expr->op_) + this->repr_expr(expr->expr_);
}

std::string ASTPrettyPrinter::repr_binary(std::shared_ptr<BinaryExpression> expr)
{
	return this->repr_expr(expr->left_) + " " + 
		TokenPrinter::print_operator(expr->op_) + " " +
		this->repr_expr(expr->right_);
}

std::string ASTPrettyPrinter::repr_ternary(std::shared_ptr<TernaryExpression> expr)
{
	return this->repr_expr(expr->cond_) + " ? " +
		this->repr_expr(expr->left_) + " : " +
		this->repr_expr(expr->right_);
}

std::string ASTPrettyPrinter::repr_assignment(std::shared_ptr<AssignmentExpression> expr)
{
	return expr->identifier_ + "=" + this->repr_expr(expr->right_);
}
