#include "ASTDebugPrinter.h"

void ASTDebugPrinter::print_expression(expr_ptr expr)
{
	this->os_ << this->repr_expr(expr) << std::endl;
}

std::string ASTDebugPrinter::repr_expr(expr_ptr expr)
{
	switch (expr->type())
	{
	case ExprType::EXPRESSION_ERROR:
		return "(ERROR)";
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
		return "( INVALID_EXPRESSION )";
	}
}

std::string ASTDebugPrinter::repr_grouping(std::shared_ptr<GroupingExpression> expr)
{
	return "( Group " + this->repr_expr(expr->expr()) + " )";
}

std::string ASTDebugPrinter::repr_primary(std::shared_ptr<PrimaryExpression> expr)
{
	switch (expr->primary_type())
	{
	case PrimaryType::PRIMARY_BOOL:
		return "( BOOL " + std::string(expr->val().bool_val ? "True" : "False") + " )";
	case PrimaryType::PRIMARY_NUM:
		return "( NUM " + std::to_string(expr->val().num_val) + " )";
	case PrimaryType::PRIMARY_STR:
		return "( STR " + * expr->val().str_val + " )";
	case PrimaryType::PRIMARY_IDENTIFIER:
		return "( IDENTIFIER " + * expr->val().identifier_val + " )";
	default:
		return "( INVALID_PRIMARY )";
	}
}

std::string ASTDebugPrinter::repr_unary(std::shared_ptr<UnaryExpression> expr)
{
	return "( " + TokenPrinter::print_operator(expr->op()) + " " + this->repr_expr(expr->expr()) + " )";
}

std::string ASTDebugPrinter::repr_binary(std::shared_ptr<BinaryExpression> expr)
{
	return "( " + TokenPrinter::print_operator(expr->op()) + " " +
		this->repr_expr(expr->left()) + " " +
		this->repr_expr(expr->right()) + " )";
}

std::string ASTDebugPrinter::repr_ternary(std::shared_ptr<TernaryExpression> expr)
{
	return "( ? " + this->repr_expr(expr->cond()) + " : " +
		this->repr_expr(expr->left()) + " " +
		this->repr_expr(expr->right()) + " )";
}

std::string ASTDebugPrinter::repr_assignment(std::shared_ptr<AssignmentExpression> expr)
{
	//TODO: Fix this
	// return "( = " + expr->identifier() + " " + this->repr_expr(expr->expr()) + " )";
	return "TODO";
}
