#include "Interpreter.h"

Interpreter::Interpreter(program_t& program)
	: program_(program) {}

void Interpreter::run()
{
	for (expr_ptr expr : program_)
	{
		this->run_expr(expr);
	}
}

Object Interpreter::run_single_expression(expr_ptr expr)
{
	return this->run_expr(expr);
}

Object Interpreter::run_expr(expr_ptr expr)
{
	switch (expr->type())
	{
	case ExprType::EXPRESSION_ERROR:
		return Object::create_error_object();
	case ExprType::EXPRESSION_GROUPING:
		return this->run_grouping_expr(std::dynamic_pointer_cast<GroupingExpression, Expression>(expr));
	case ExprType::EXPRESSION_PRIMARY:
		return this->run_primary_expr(std::dynamic_pointer_cast<PrimaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_UNARY:
		return this->run_unary_expr(std::dynamic_pointer_cast<UnaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_BINARY:
		return this->run_binary_expr(std::dynamic_pointer_cast<BinaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_TERNARY:
		return this->run_ternary_expr(std::dynamic_pointer_cast<TernaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_ASSIGNMENT:
		return this->run_assignment_expr(std::dynamic_pointer_cast<AssignmentExpression, Expression>(expr));
	default:
		//TODO: Add Error Reporting
		return Object::create_error_object();
	}
}

Object Interpreter::run_grouping_expr(std::shared_ptr<GroupingExpression> expr)
{
	return this->run_expr(expr->expr());
}

Object Interpreter::run_primary_expr(std::shared_ptr<PrimaryExpression> expr)
{
	switch (expr->primary_type())
	{
	case PrimaryType::PRIMARY_BOOL:
		return Object::create_bool_object(expr->val().bool_val);
	case PrimaryType::PRIMARY_NUM:
		return Object::create_num_object(expr->val().num_val);
	case PrimaryType::PRIMARY_STR:
		return Object::create_str_object(*expr->val().str_val);
	case PrimaryType::PRIMARY_IDENTIFIER:
		//TODO: Add variables
	default:
		//TODO: Add Error Reporting
		return Object::create_error_object();
	}
}

Object Interpreter::run_unary_expr(std::shared_ptr<UnaryExpression> expr)
{
	Object inner = this->run_expr(expr->expr());
	switch (expr->op())
	{
	case Operator::OPERATOR_BANG:
		return Object::create_bool_object(!inner.to_bool().bool_value());
	case Operator::OPERATOR_SUBTRACT:
		if (inner.is_num())
		{
			return Object::create_num_object(-inner.num_value());
		}
		else
		{ 
			//TODO: Error reporting
			return Object::create_error_object();
		}
	default:
		//TODO: Add Error Reporting
		return Object::create_error_object();
	}
}

Object Interpreter::run_binary_expr(std::shared_ptr<BinaryExpression> expr)
{
	Object left = this->run_expr(expr->left());
	Object right = this->run_expr(expr->right());

	switch (expr->op())
	{
	case Operator::OPERATOR_ADD:
		if (left.is_num() && right.is_num())
		{
			return Object::create_num_object(left.num_value() + right.num_value());
		}
		else
		{ 
			//TODO: Error reporting
			return Object::create_error_object();
		}
	case Operator::OPERATOR_SUBTRACT:
		if (left.is_num() && right.is_num())
		{
			return Object::create_num_object(left.num_value() - right.num_value());
		}
		else
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
	case Operator::OPERATOR_MULTIPLY:
		if (left.is_num() && right.is_num())
		{
			return Object::create_num_object(left.num_value() * right.num_value());
		}
		else
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
	case Operator::OPERATOR_DIVIDE:
		if (left.is_num() && right.is_num())
		{
			return Object::create_num_object(left.num_value() / right.num_value());
		}
		else
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
	case Operator::OPERATOR_GREATER:
		if (left.is_num() && right.is_num())
		{
			return Object::create_bool_object(left.num_value() > right.num_value());
		}
		else
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
	case Operator::OPERATOR_LESS:
		if (left.is_num() && right.is_num())
		{
			return Object::create_bool_object(left.num_value() < right.num_value());
		}
		else
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
	case Operator::OPERATOR_GREATER_EQUAL:
		if (left.is_num() && right.is_num())
		{
			return Object::create_bool_object(left.num_value() >= right.num_value());
		}
		else
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
	case Operator::OPERATOR_LESS_EQUAL:
		if (left.is_num() && right.is_num())
		{
			return Object::create_bool_object(left.num_value() <= right.num_value());
		}
		else
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
	case Operator::OPERATOR_EQUAL_EQUAL:
	{
		if (left.type() != right.type())
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
		switch (left.type())
		{
		case ObjType::BOOL:
			return Object::create_bool_object(left.bool_value() == right.bool_value());
		case ObjType::NUM:
			return Object::create_bool_object(left.num_value() == right.num_value());
		case ObjType::STR:
			return Object::create_bool_object(left.str_value() == right.str_value());
		default:
			return Object::create_bool_object(true);
		}
	}
	case Operator::OPERATOR_NOT_EQUAL:
	{
		if (left.type() != right.type())
		{
			//TODO: Error reporting
			return Object::create_error_object();
		}
		switch (left.type())
		{
		case ObjType::BOOL:
			return Object::create_bool_object(left.bool_value() != right.bool_value());
		case ObjType::NUM:
			return Object::create_bool_object(left.num_value() != right.num_value());
		case ObjType::STR:
			return Object::create_bool_object(left.str_value() != right.str_value());
		default:
			return Object::create_bool_object(false);
		}
	}
	case Operator::OPERATOR_AND:
		return Object::create_bool_object(left.to_bool().bool_value() && right.to_bool().bool_value());
	case Operator::OPERATOR_OR:
		return Object::create_bool_object(left.to_bool().bool_value() || right.to_bool().bool_value());
	}
}

Object Interpreter::run_ternary_expr(std::shared_ptr<TernaryExpression> expr)
{
	Object cond = this->run_expr(expr->cond());

	if (cond.to_bool().bool_value())
	{
		return this->run_expr(expr->left());
	}
	return this->run_expr(expr->right());
}

Object Interpreter::run_assignment_expr(std::shared_ptr<AssignmentExpression> expr)
{
	//TODO: Add assignment
	return Object::create_error_object();
}
