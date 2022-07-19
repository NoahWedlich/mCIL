#include "ASTDebugPrinter.h"

void ASTDebugPrinter::print_expression(Expression* expr)
{
	this->os_ << this->repr_expr(expr) << std::endl;
}

std::string ASTDebugPrinter::repr_expr(Expression* expr)
{
	switch (expr->type_)
	{
	case ExprType::EXPRESSION_ERROR:
		return "(ERROR)";
	case ExprType::EXPRESSION_GROUPING:
		return this->repr_grouping(dynamic_cast<GroupingExpression*>(expr));
	case ExprType::EXPRESSION_PRIMARY:
		return this->repr_primary(dynamic_cast<PrimaryExpression*>(expr));
	case ExprType::EXPRESSION_UNARY:
		return this->repr_unary(dynamic_cast<UnaryExpression*>(expr));
	case ExprType::EXPRESSION_BINARY:
		return this->repr_binary(dynamic_cast<BinaryExpression*>(expr));
	case ExprType::EXPRESSION_TERNARY:
		return this->repr_ternary(dynamic_cast<TernaryExpression*>(expr));
	case ExprType::EXPRESSION_ASSIGNMENT:
		return this->repr_assignment(dynamic_cast<AssignmentExpression*>(expr));
	default:
		return "( INVALID_EXPRESSION )";
	}
}

std::string ASTDebugPrinter::repr_grouping(GroupingExpression* expr)
{
	return "( Group " + this->repr_expr(expr->expr_) + " )";
}

std::string ASTDebugPrinter::repr_primary(PrimaryExpression* expr)
{
	switch (expr->primary_type_)
	{
	case PrimaryType::PRIMARY_BOOL:
		return "( BOOL " + std::string(expr->val_.bool_val ? "True" : "False") + " )";
	case PrimaryType::PRIMARY_NUM:
		return "( NUM " + std::to_string(expr->val_.num_val) + " )";
	case PrimaryType::PRIMARY_STR:
		return "( STR " + * expr->val_.str_val + " )";
	case PrimaryType::PRIMARY_IDENTIFIER:
		return "( IDENTIFIER " + * expr->val_.identifier_val + " )";
	default:
		return "( INVALID_PRIMARY )";
	}
}

std::string ASTDebugPrinter::repr_unary(UnaryExpression* expr)
{
	return "( " + TokenPrinter::print_operator(expr->op_) + " " + this->repr_expr(expr->expr_) + " )";
}

std::string ASTDebugPrinter::repr_binary(BinaryExpression* expr)
{
	return "( " + TokenPrinter::print_operator(expr->op_) + " " +
		this->repr_expr(expr->left_) + " " +
		this->repr_expr(expr->right_) + " )";
}

std::string ASTDebugPrinter::repr_ternary(TernaryExpression* expr)
{
	return "( ? " + this->repr_expr(expr->cond_) + " : " +
		this->repr_expr(expr->left_) + " " +
		this->repr_expr(expr->right_) + " )";
}

std::string ASTDebugPrinter::repr_assignment(AssignmentExpression* expr)
{
	return "( = " + expr->identifier_ + " " + this->repr_expr(expr->right_) + " )";
}
