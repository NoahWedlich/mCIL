#include "Interpreter.h"

Interpreter::Interpreter()
	: program_(), env_() {}

Interpreter::Interpreter(stmt_list& program)
	: program_(program), env_() {}

void Interpreter::run()
{
	for (stmt_ptr stmt : program_)
	{
		try
		{
			this->run_stmt(stmt);
		}
		catch (CILError& err)
		{
			if (!err.has_pos())
			{ err.add_range(stmt->pos()); }
			ErrorManager::cil_error(err);
			break;
		}
	}
}

void Interpreter::run_single_statement(stmt_ptr stmt)
{
	try
	{
		this->run_stmt(stmt);
	}
	catch (CILError& err)
	{
		if (!err.has_pos())
		{ err.add_range(stmt->pos()); }
		ErrorManager::cil_error(err);
	}
}

Object Interpreter::run_single_expression(expr_ptr expr)
{
	try
	{
		return this->run_expr(expr);
	}
	catch (CILError& err)
	{
		if (!err.has_pos())
		{ err.add_range(expr->pos()); }
		ErrorManager::cil_error(err);
		return Object::create_error_object();
	}
}


void Interpreter::assert_binary_types(Operator op, Object left, Object right, ObjType left_t, ObjType right_t, Position pos)
{
	if (left.type() != left_t)
	{
		if (right.type() != right_t)
		{
			throw CILError::error(pos, "Operands of binary '$' must be '$' and '$'. Got '$' and '$'",
				op, left_t, right_t, left.type(), right.type());
		}
		throw CILError::error(pos, "left operand of binary '$' must be '$' not '$'.",
			op, left_t, left.type());
	}
	if (right.type() != right_t)
	{
		throw CILError::error(pos, "right operand of binary '$' must be '$' not '$'.",
			op, right_t, right.type());
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
	case ExprType::EXPRESSION_CALL:
		return this->run_call_expr(std::dynamic_pointer_cast<CallExpression, Expression>(expr));
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
		throw CILError::error(expr->pos(), "Incomplete handling of expressions");
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
	{
		try
		{
			return this->env_.get_var(*expr->val().identifier_val).value;
		}
		catch (CILError& err)
		{
			if (!err.has_pos())
			{ err.add_range(expr->pos()); }
			ErrorManager::cil_error(err);
			return Object::create_error_object();
		}
	}
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of primary expressions");
	}
}

Object Interpreter::run_call_expr(std::shared_ptr<CallExpression> expr)
{
	try
	{
		Function func = this->env_.get_func(expr->identifier());
		std::vector<Variable> args {};
		if (expr->args().size() != func.info.args.size())
		{
			throw CILError::error(expr->pos(), "Function '$' expects $ arguments, got $",
				func.info.name.c_str(), func.info.args.size(), expr->args().size());
		}
		for (int i = 0; i < func.info.args.size(); i++)
		{
			Object arg_val = this->run_expr(expr->args()[i]);
			VarInfo arg_info = func.info.args[i];
			if (arg_val.type() != arg_info.type)
			{
				throw CILError::error(expr->pos(), "Argument '$' of function '$' must be '$' not '$'",
					arg_info.name.c_str(), func.info.name.c_str(), arg_info.type, arg_val.type());
			}
			Variable arg{ arg_info, arg_val};
			args.push_back(arg);
		}

		Environment previous = this->env_;
		this->env_ = Environment(&previous);
		for (Variable arg : args)
		{
			this->env_.define_var(arg);
		}

		try
		{
			this->run_stmt(func.body);
		}
		catch (Return& ret)
		{
			this->env_ = previous;
			return ret.ret_val();
		}

		this->env_ = previous;
	}
	catch (CILError& err)
	{
		if (!err.has_pos())
		{ err.add_range(expr->pos()); }
		ErrorManager::cil_error(err);
		return Object::create_error_object();
	}
}

Object Interpreter::run_unary_expr(std::shared_ptr<UnaryExpression> expr)
{
	Object inner = this->run_expr(expr->expr());
	if (inner.is_err())
	{ return Object::create_error_object(); }
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
			throw CILError::error(expr->pos(), "Operand of unary '$' must be '$' not '$'",
				expr->op(), ObjType::NUM, inner.type());
		}
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of unary expressions");
	}
}

Object Interpreter::run_binary_expr(std::shared_ptr<BinaryExpression> expr)
{
	Object left = this->run_expr(expr->left());
	if (left.is_err())
	{ return Object::create_error_object(); }
	Object right = this->run_expr(expr->right());
	if (right.is_err())
	{ return Object::create_error_object(); }

	switch (expr->op())
	{
	case Operator::OPERATOR_ADD:
		this->assert_binary_types(expr->op(), left, right, ObjType::NUM, ObjType::NUM, expr->pos());
		return Object::create_num_object(left.num_value() + right.num_value());
	case Operator::OPERATOR_SUBTRACT:
		this->assert_binary_types(expr->op(), left, right, ObjType::NUM, ObjType::NUM, expr->pos());
		return Object::create_num_object(left.num_value() - right.num_value());
	case Operator::OPERATOR_MULTIPLY:
		this->assert_binary_types(expr->op(), left, right, ObjType::NUM, ObjType::NUM, expr->pos());
		return Object::create_num_object(left.num_value() * right.num_value());
	case Operator::OPERATOR_DIVIDE:
		this->assert_binary_types(expr->op(), left, right, ObjType::NUM, ObjType::NUM, expr->pos());
		return Object::create_num_object(left.num_value() / right.num_value());
	case Operator::OPERATOR_GREATER:
		this->assert_binary_types(expr->op(), left, right, ObjType::NUM, ObjType::NUM, expr->pos());
		return Object::create_bool_object(left.num_value() > right.num_value());
	case Operator::OPERATOR_LESS:
		this->assert_binary_types(expr->op(), left, right, ObjType::NUM, ObjType::NUM, expr->pos());
		return Object::create_bool_object(left.num_value() < right.num_value());
	case Operator::OPERATOR_GREATER_EQUAL:
		this->assert_binary_types(expr->op(), left, right, ObjType::NUM, ObjType::NUM, expr->pos());
		return Object::create_bool_object(left.num_value() >= right.num_value());
	case Operator::OPERATOR_LESS_EQUAL:
		this->assert_binary_types(expr->op(), left, right, ObjType::NUM, ObjType::NUM, expr->pos());
		return Object::create_bool_object(left.num_value() <= right.num_value());
	case Operator::OPERATOR_EQUAL_EQUAL:
	{
		if (left.type() != right.type())
		{
			throw CILError::error(expr->pos(), "Operands for binary '$' must be the same, got '$' and '$'",
				expr->op(), left.type(), right.type());
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
			throw CILError::error(expr->pos(), "Operands for binary '$' must be the same, got '$' and '$'",
				expr->op(), left.type(), right.type());
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
		throw CILError::error(expr->pos(), "Incomplete handling of binary expressions");
	}
}

Object Interpreter::run_ternary_expr(std::shared_ptr<TernaryExpression> expr)
{
	Object cond = this->run_expr(expr->cond());
	if (cond.is_err())
	{ return Object::create_error_object(); }

	if (cond.to_bool().bool_value())
	{
		return this->run_expr(expr->left());
	}
	return this->run_expr(expr->right());
}

Object Interpreter::run_assignment_expr(std::shared_ptr<AssignmentExpression> expr)
{
	Object value = this->run_expr(expr->expr());
	if (value.is_err())
	{ return Object::create_error_object(); }

	try
	{
		this->env_.assign_var(expr->identifier(), value);
	}
	catch (CILError& err)
	{
		if (!err.has_pos())
		{ err.add_range(expr->pos()); }
		throw err;
	}
	return value;
}

void Interpreter::run_stmt(stmt_ptr stmt)
{
	switch (stmt->type())
	{
	case StmtType::STATEMENT_BLOCK:
		this->run_block_stmt(std::dynamic_pointer_cast<BlockStatement, Statement>(stmt));
		break;
	case StmtType::STATEMENT_RETURN:
		this->run_return_stmt(std::dynamic_pointer_cast<ReturnStatement, Statement>(stmt));
		break;
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
	case StmtType::STATEMENT_VAR_DECL:
		this->run_var_decl_stmt(std::dynamic_pointer_cast<VarDeclStatement, Statement>(stmt));
		break;
	case StmtType::STATEMENT_FUNC_DECL:
		this->run_func_decl_stmt(std::dynamic_pointer_cast<FuncDeclStatement, Statement>(stmt));
		break;
	case StmtType::STATEMENT_EXPR:
		this->run_expr_stmt(std::dynamic_pointer_cast<ExprStatement, Statement>(stmt));
		break;
	default:
		throw CILError::error(stmt->pos(), "Incomplete handling of statements");
	}
}

void Interpreter::run_block_stmt(std::shared_ptr<BlockStatement> stmt)
{
	Environment previous = this->env_;
	this->env_ = Environment(&previous);
	for (stmt_ptr inner : stmt->inner())
	{
		this->run_stmt(inner);
	}
	this->env_ = previous;
}

void Interpreter::run_return_stmt(std::shared_ptr<ReturnStatement> stmt)
{
	Object val = this->run_expr(stmt->expr());
	throw Return(val);
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
	case ObjType::ERROR:
		break;
	default:
		throw CILError::error(stmt->pos(), "Cannot print value of type '$'", val.type());
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
		this->run_expr(stmt->exec())
		)
	{
		this->run_stmt(stmt->inner());
	}
}

void Interpreter::run_var_decl_stmt(std::shared_ptr<VarDeclStatement> stmt)
{
	Object value = this->run_expr(stmt->val());

	if (stmt->info().type != ObjType::UNKNOWN && stmt->info().type != value.type())
	{
		throw CILError::error(stmt->pos(), "Cannot initialize variable of type '$' with value of type '$'",
			stmt->info().type, value.type());
	}
	
	Variable var{ stmt->info(), value};

	this->env_.define_var(var);
}

void Interpreter::run_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt)
{
	Function func{ stmt->info(), stmt->body() };
	this->env_.define_func(func);
}

void Interpreter::run_expr_stmt(std::shared_ptr<ExprStatement> stmt)
{
	this->run_expr(stmt->expr());
}