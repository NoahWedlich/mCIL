#include "Interpreter.h"

Interpreter::Interpreter()
	: program_(), env_() 
{
	this->env_ = new Environment();
}

Interpreter::Interpreter(stmt_list& program)
	: program_(program), env_()
{
	this->env_ = new Environment();
}

Interpreter::~Interpreter()
{
	delete this->env_;
}

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

value_ptr Interpreter::run_single_expression(expr_ptr expr)
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
		return CIL::ErrorValue::create();
	}
}

value_ptr Interpreter::run_expr(expr_ptr expr)
{
	switch (expr->type())
	{
	case ExprType::EXPRESSION_ERROR:
		return CIL::ErrorValue::create();
	case ExprType::EXPRESSION_GROUPING:
		return this->run_grouping_expr(std::dynamic_pointer_cast<GroupingExpression, Expression>(expr));
	case ExprType::EXPRESSION_CALL:
		return this->run_call_expr(std::dynamic_pointer_cast<CallExpression, Expression>(expr));
	case ExprType::EXPRESSION_ARRAY_ACCESS:
		return this->run_array_access_expr(std::dynamic_pointer_cast<ArrayAccessExpression, Expression>(expr));
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

value_ptr Interpreter::run_grouping_expr(std::shared_ptr<GroupingExpression> expr)
{
	return this->run_expr(expr->expr());
}

value_ptr Interpreter::run_primary_expr(std::shared_ptr<PrimaryExpression> expr)
{
	switch (expr->primary_type())
	{
	case PrimaryType::PRIMARY_BOOL:
		return CIL::Bool::create(expr->val().bool_val);
	case PrimaryType::PRIMARY_NUM:
		return CIL::Number::create(expr->val().num_val);
	case PrimaryType::PRIMARY_STR:
		return CIL::String::create(*expr->val().str_val);
	case PrimaryType::PRIMARY_IDENTIFIER:
	{
		try
		{
			return this->env_->get_var(*expr->val().identifier_val).value;
		}
		catch (CILError& err)
		{
			if (!err.has_pos())
			{ err.add_range(expr->pos()); }
			ErrorManager::cil_error(err);
			return CIL::ErrorValue::create();
		}
	}
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of primary expressions");
	}
}

value_ptr Interpreter::run_call_expr(std::shared_ptr<CallExpression> expr)
{
	try
	{
		Function func = this->env_->get_func(expr->identifier());
		std::vector<Variable> args {};
		if (expr->args().size() != func.info.args.size())
		{
			throw CILError::error(expr->pos(), "Function '$' expects $ arguments, got $",
				func.info.name.c_str(), func.info.args.size(), expr->args().size());
		}
		for (int i = 0; i < func.info.args.size(); i++)
		{
			value_ptr arg_val = this->run_expr(expr->args()[i]);
			VarInfo arg_info = func.info.args[i];
			if (arg_val->type() != arg_info.type)
			{
				throw CILError::error(expr->pos(), "Argument '$' of function '$' must be '$' not '$'",
					arg_info.name.c_str(), func.info.name.c_str(), arg_info.type, arg_val->type());
			}
			Variable arg{ arg_info, arg_val};
			args.push_back(arg);
		}

		Environment* previous = this->env_;
		this->env_ = new Environment(previous);
		for (Variable arg : args)
		{
			this->env_->define_var(arg);
		}

		try
		{
			this->run_stmt(func.body);
		}
		catch (Return& ret)
		{
			delete this->env_;
			this->env_ = previous;
			if (ret.ret_val()->type() != func.info.ret_type)
			{
				throw CILError::error(func.body->pos(), "Function '$' should return '$' not '$'",
					func.info.name, func.info.ret_type, ret.ret_val()->type());
			}
			return ret.ret_val();
		}
		delete this->env_;
		this->env_ = previous;
		//Change to none
		return CIL::ErrorValue::create();
	}
	catch (CILError& err)
	{
		if (!err.has_pos())
		{ err.add_range(expr->pos()); }
		ErrorManager::cil_error(err);
		return CIL::ErrorValue::create();
	}
}

value_ptr Interpreter::run_array_access_expr(std::shared_ptr<ArrayAccessExpression> expr)
{
	value_ptr index_num = this->run_expr(expr->index());
	if(index_num->type() != Type::NUM)
	{ throw CILError::error(expr->pos(), "Index must be 'num' not '$'", index_num->type()); }
	int index = (int)std::dynamic_pointer_cast<CIL::Number>(index_num)->value();
	Array arr = this->env_->get_arr(expr->identifier());
	if (index < 0 || index >= arr.info.size)
	{ throw CILError::error(expr->pos(), "Index must be in the range [$,$[", 0, arr.info.size); }
	return arr.arr[index];
}

value_ptr Interpreter::run_unary_expr(std::shared_ptr<UnaryExpression> expr)
{
	value_ptr inner = this->run_expr(expr->expr());
	if (inner->type() == Type::ERROR)
	{ return CIL::ErrorValue::create(); }
	switch (expr->op())
	{
	case Operator::OPERATOR_BANG:
		TRY_OP(return inner->invert(), expr->pos());
	case Operator::OPERATOR_SUBTRACT:
		TRY_OP(return inner->invert(), expr->pos());
	case Operator::OPERATOR_INCREMENT:
		TRY_OP(return inner->increment(), expr->pos());
	case Operator::OPERATOR_DECREMENT:
		TRY_OP(return inner->decrement(), expr->pos());
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of unary expressions");
	}
}

value_ptr Interpreter::run_binary_expr(std::shared_ptr<BinaryExpression> expr)
{
	value_ptr left = this->run_expr(expr->left());
	if (left->type() == Type::ERROR)
	{ return CIL::ErrorValue::create(); }
	value_ptr right = this->run_expr(expr->right());
	if (right->type() == Type::ERROR)
	{ return CIL::ErrorValue::create(); }

	switch (expr->op())
	{
	case Operator::OPERATOR_ADD:
		TRY_OP(return left->add(right), expr->pos());
	case Operator::OPERATOR_SUBTRACT:
		TRY_OP(return left->subtract(right), expr->pos());
	case Operator::OPERATOR_MULTIPLY:
		TRY_OP(return left->multiply(right), expr->pos());
	case Operator::OPERATOR_DIVIDE:
		TRY_OP(return left->divide(right), expr->pos());
	case Operator::OPERATOR_LEFT_BITSHIFT:
		TRY_OP(return left->left_bitshift(right), expr->pos());
	case Operator::OPERATOR_RIGHT_BITSHIFT:
		TRY_OP(return left->right_bitshift(right), expr->pos());
	case Operator::OPERATOR_GREATER:
		TRY_OP(return left->greater(right), expr->pos());
	case Operator::OPERATOR_LESS:
		TRY_OP(return left->less(right), expr->pos());
	case Operator::OPERATOR_GREATER_EQUAL:
		TRY_OP(return left->greater_equals(right), expr->pos());
	case Operator::OPERATOR_LESS_EQUAL:
		TRY_OP(return left->less_equals(right), expr->pos());
	case Operator::OPERATOR_EQUAL_EQUAL:
		TRY_OP(return left->equals(right), expr->pos());
	case Operator::OPERATOR_NOT_EQUAL:
		TRY_OP(return left->not_equals(right), expr->pos());
	case Operator::OPERATOR_AND:
		TRY_OP(return left->logical_and(right), expr->pos());
	case Operator::OPERATOR_OR:
		TRY_OP(return left->logical_or(right), expr->pos());
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of binary expressions");
	}
}

value_ptr Interpreter::run_ternary_expr(std::shared_ptr<TernaryExpression> expr)
{
	value_ptr cond = this->run_expr(expr->cond());
	if (cond->type() == Type::ERROR)
	{ return CIL::ErrorValue::create(); }

	if (cond->to_bool())
	{
		return this->run_expr(expr->left());
	}
	return this->run_expr(expr->right());
}

value_ptr Interpreter::run_assignment_expr(std::shared_ptr<AssignmentExpression> expr)
{
	value_ptr value = this->run_expr(expr->expr());
	if (value->type() == Type::ERROR)
	{ return CIL::ErrorValue::create(); }

	value_ptr target = this->run_expr(expr->target());
	TRY_OP(return target->assign(value), expr->pos());
}

void Interpreter::run_stmt(stmt_ptr stmt)
{
	switch (stmt->type())
	{
	case StmtType::STATEMENT_BLOCK:
		this->run_block_stmt(std::dynamic_pointer_cast<BlockStatement, Statement>(stmt));
		break;
	case StmtType::STATEMENT_BREAK:
		this->run_break_stmt(std::dynamic_pointer_cast<BreakStatement, Statement>(stmt));
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
	case StmtType::STATEMENT_ARR_DECL:
		this->run_arr_decl_stmt(std::dynamic_pointer_cast<ArrDeclStatement, Statement>(stmt));
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
	Environment* previous = this->env_;
	this->env_ = new Environment(previous);
	for (stmt_ptr inner : stmt->inner())
	{
		try
		{
			this->run_stmt(inner);
		}
		catch (Return& ret)
		{
			delete this->env_;
			this->env_ = previous;
			throw ret;
		}
	}
	delete this->env_;
	this->env_ = previous;
}

void Interpreter::run_break_stmt(std::shared_ptr<BreakStatement> stmt)
{
	throw Break();
}

void Interpreter::run_return_stmt(std::shared_ptr<ReturnStatement> stmt)
{
	value_ptr val = this->run_expr(stmt->expr());
	throw Return(val);
}

void Interpreter::run_print_stmt(std::shared_ptr<PrintStatement> stmt)
{
	value_ptr val = this->run_expr(stmt->expr());
	std::cout << val->to_string() << std::endl;
}

void Interpreter::run_if_stmt(std::shared_ptr<IfStatement> stmt)
{
	value_ptr val = this->run_expr(stmt->cond());
	if (val->to_bool())
	{
		this->run_stmt(stmt->if_branch());
	}
	else if (stmt->else_branch() != nullptr)
	{
		run_stmt(stmt->else_branch());
	}
}

void Interpreter::run_while_stmt(std::shared_ptr<WhileStatement> stmt)
{
	try
	{
		while (this->run_expr(stmt->cond())->to_bool())
		{
			this->run_stmt(stmt->inner());
		}
	}
	catch (Break)
	{
	}
}

void Interpreter::run_for_stmt(std::shared_ptr<ForStatement> stmt)
{
	try
	{
		for (
			this->run_stmt(stmt->init());
			this->run_expr(stmt->cond())->to_bool();
			this->run_expr(stmt->exec())
			)
		{
			this->run_stmt(stmt->inner());
		}
	}
	catch (Break)
	{
	}
}

void Interpreter::run_var_decl_stmt(std::shared_ptr<VarDeclStatement> stmt)
{
	value_ptr value = this->run_expr(stmt->val());

	if (stmt->info().type.type != Type::UNKNOWN && stmt->info().type != value->type())
	{
		throw CILError::error(stmt->pos(), "Cannot initialize variable of type '$' with value of type '$'",
			stmt->info().type, value->type());
	}
	
	Variable var{ stmt->info(), value};

	this->env_->define_var(var);
}

void Interpreter::run_arr_decl_stmt(std::shared_ptr<ArrDeclStatement> stmt)
{
	if (stmt->info().size != stmt->vals().size())
	{
		throw CILError::error(stmt->pos(), "Array of size '$' cannot be initialized with '$' elements",
			stmt->info().size, stmt->vals().size());
	}
	std::vector<value_ptr> vals{};
	for (expr_ptr expr : stmt->vals())
	{
		value_ptr val = this->run_expr(expr);
		if (val->type() != stmt->info().type)
		{
			throw CILError::error(stmt->pos(), "Cannot initizalize array of type '$' with value of type '$'",
				stmt->info().type, val->type());
		}
		vals.push_back(val);
	}
	
	Array arr{ stmt->info(), vals };
	this->env_->define_arr(arr);
}

void Interpreter::run_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt)
{
	Function func{ stmt->info(), stmt->body() };
	this->env_->define_func(func);
}

void Interpreter::run_expr_stmt(std::shared_ptr<ExprStatement> stmt)
{
	this->run_expr(stmt->expr());
}