#include "Interpreter.h"

Interpreter::Interpreter(decl_list& program)
	: program_(program), env_() {}

void Interpreter::run()
{
	for (decl_ptr decl : program_)
	{
		try
		{
			this->run_decl(decl);
		}
		catch (InterpreterError& err)
		{
			ErrorManager::cil_interpreter_error(err);
			break;
		}
	}
}

void Interpreter::run_single_declaration(decl_ptr decl)
{
	try
	{
		this->run_decl(decl);
	}
	catch (InterpreterError& err)
	{
		ErrorManager::cil_interpreter_error(err);
	}
}

void Interpreter::run_single_statement(stmt_ptr stmt)
{
	try
	{
		this->run_stmt(stmt);
	}
	catch (InterpreterError& err)
	{
		ErrorManager::cil_interpreter_error(err);
	}
}

Object Interpreter::run_single_expression(expr_ptr expr)
{
	try
	{
		return this->run_expr(expr);
	}
	catch (InterpreterError& err)
	{
		ErrorManager::cil_interpreter_error(err);
		return Object::create_error_object();
	}
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
		throw InterpreterError("Unreachable!", *expr);
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
		return this->env_.get(*expr->val().identifier_val);
	default:
		throw InterpreterError("Unreachable", *expr);
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
			throw InterpreterError("Operand for unary '-' must be num", *expr);
		}
	default:
		throw InterpreterError("Unreachable", *expr);
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
			throw InterpreterError("Operands for binary '+' must be nums", *expr);
		}
	case Operator::OPERATOR_SUBTRACT:
		if (left.is_num() && right.is_num())
		{
			return Object::create_num_object(left.num_value() - right.num_value());
		}
		else
		{
			throw InterpreterError("Operands for binary '-' must be nums", *expr);
		}
	case Operator::OPERATOR_MULTIPLY:
		if (left.is_num() && right.is_num())
		{
			return Object::create_num_object(left.num_value() * right.num_value());
		}
		else
		{
			throw InterpreterError("Operands for binary '*' must be nums", *expr);
		}
	case Operator::OPERATOR_DIVIDE:
		if (left.is_num() && right.is_num())
		{
			return Object::create_num_object(left.num_value() / right.num_value());
		}
		else
		{
			throw InterpreterError("Operands for binary '/' must be nums", *expr);
		}
	case Operator::OPERATOR_GREATER:
		if (left.is_num() && right.is_num())
		{
			return Object::create_bool_object(left.num_value() > right.num_value());
		}
		else
		{
			throw InterpreterError("Operands for binary '>' must be nums", *expr);
		}
	case Operator::OPERATOR_LESS:
		if (left.is_num() && right.is_num())
		{
			return Object::create_bool_object(left.num_value() < right.num_value());
		}
		else
		{
			throw InterpreterError("Operands for binary '<' must be nums", *expr);
		}
	case Operator::OPERATOR_GREATER_EQUAL:
		if (left.is_num() && right.is_num())
		{
			return Object::create_bool_object(left.num_value() >= right.num_value());
		}
		else
		{
			throw InterpreterError("Operands for binary '>=' must be nums", *expr);
		}
	case Operator::OPERATOR_LESS_EQUAL:
		if (left.is_num() && right.is_num())
		{
			return Object::create_bool_object(left.num_value() <= right.num_value());
		}
		else
		{
			throw InterpreterError("Operands for binary '<=' must be nums", *expr);
		}
	case Operator::OPERATOR_EQUAL_EQUAL:
	{
		if (left.type() != right.type())
		{
			throw InterpreterError("Operands for binary '==' must be the same", *expr);
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
			throw InterpreterError("Operands for binary '!=' must be the same", *expr);
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
	default:
		throw InterpreterError("Unreachable", *expr);
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

void Interpreter::run_stmt(stmt_ptr stmt)
{
	switch (stmt->type())
	{
	case StmtType::STATEMENT_PRINT:
		this->run_print_stmt(std::dynamic_pointer_cast<PrintStatement, Statement>(stmt));
		break;
	case StmtType::STATEMENT_IF:
		this->run_if_stmt(std::dynamic_pointer_cast<IfStatement, Statement>(stmt));
		break;
	case StmtType::STATEMENT_WHILE:
		this->run_while_stmt(std::dynamic_pointer_cast<WhileStatement, Statement>(stmt));
		break;
	case StmtType::STATEMENT_FOR:
		this->run_for_stmt(std::dynamic_pointer_cast<ForStatement, Statement>(stmt));
		break;
	case StmtType::STATEMENT_EXPR:
		this->run_expr_stmt(std::dynamic_pointer_cast<ExprStatement, Statement>(stmt));
		break;
	default:
		throw InterpreterError("Unreachable!", *stmt);
	}
}

void Interpreter::run_print_stmt(std::shared_ptr<PrintStatement> stmt)
{
	Object val = this->run_expr(stmt->expr());
	switch (val.type())
	{
	case ObjType::BOOL:
		std::cout << (val.bool_value() ? "true" : "false") << std::endl;
		break;
	case ObjType::NONE:
		std::cout << "None" << std::endl;
		break;
	case ObjType::NUM:
		std::cout << val.num_value() << std::endl;
		break;
	case ObjType::STR:
		std::cout << val.str_value() << std::endl;
		break;
	default:
		throw InterpreterError("Trying to print invalid value", *stmt);
	}
}

void Interpreter::run_if_stmt(std::shared_ptr<IfStatement> stmt)
{
	Object val = this->run_expr(stmt->cond());
	if (val.to_bool().bool_value())
	{
		this->run_stmt(stmt->if_branch());
	}
}

void Interpreter::run_while_stmt(std::shared_ptr<WhileStatement> stmt)
{
	while (this->run_expr(stmt->cond()).to_bool().bool_value())
	{
		this->run_stmt(stmt->inner());
	}
}

void Interpreter::run_for_stmt(std::shared_ptr<ForStatement> stmt)
{
	for (
		this->run_stmt(stmt->init());
		this->run_expr(stmt->cond()).to_bool().bool_value();
		this->run_stmt(stmt->exec())
		)
	{
		this->run_stmt(stmt->inner());
	}
}

void Interpreter::run_expr_stmt(std::shared_ptr<ExprStatement> stmt)
{
	this->run_expr(stmt->expr());
}

void Interpreter::run_decl(decl_ptr decl)
{
	switch (decl->type())
	{
	case DeclType::DECLARATION_VAR:
		this->run_var_decl(std::dynamic_pointer_cast<VarDeclaration, Declaration>(decl));
		break;
	case DeclType::DECLARATION_BLOCK:
		this->run_block_decl(std::dynamic_pointer_cast<BlockDeclaration, Declaration>(decl));
		break;
	case DeclType::DECLARATION_STMT:
		this->run_stmt_decl(std::dynamic_pointer_cast<StmtDeclaration, Declaration>(decl));
		break;
	default:
		throw InterpreterError("Unreachable!", *decl);
	}
}

void Interpreter::run_var_decl(std::shared_ptr<VarDeclaration> decl)
{
	Object value = this->run_expr(decl->val());

	if (decl->var_type() != ObjType::UNKNOWN && decl->var_type() != value.type())
	{ throw InterpreterError("Trying to initialize variable with invalid type", *decl); }

	//TODO: Add constness
	this->env_.define(decl->name(), value);
}


void Interpreter::run_block_decl(std::shared_ptr<BlockDeclaration> decl)
{
	Environment previous = this->env_;
	this->env_ = Environment(&previous);
	for (decl_ptr inner : decl->inner())
	{
		this->run_decl(inner);
	}
	this->env_ = previous;
}

void Interpreter::run_stmt_decl(std::shared_ptr<StmtDeclaration> decl)
{
	this->run_stmt(decl->inner());
}
