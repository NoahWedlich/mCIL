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
	case ExprType::EXPRESSION_ACCESS:
		return run_access_expr(std::dynamic_pointer_cast<AccessExpression, Expression>(expr));
	case ExprType::EXPRESSION_NEW:
		return run_new_expr(std::dynamic_pointer_cast<NewExpression, Expression>(expr));
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
	case PrimaryType::PRIMARY_NONE:
		return CIL::None::create();
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
		Environment::Function func = this->env_->get_func(expr->identifier());
		if (!func.body)
		{
			throw CILError::error(expr->pos(), "Function '$' was declared but never defined", func.name);
		}
		std::vector<Environment::Variable> args {};
		if (expr->args().size() != func.parameters.size())
		{
			//TODO: Add default arguments
			throw CILError::error(expr->pos(), "Function '$' expects $ arguments, got $",
				func.name.c_str(), func.parameters.size(), expr->args().size());
		}
		for (int i = 0; i < func.parameters.size(); i++)
		{
			value_ptr arg_val = this->run_expr(expr->args()[i]);
			Type required_type = func.parameters[i].type;
			if (!arg_val->type().is(required_type))
			{
				throw CILError::error(expr->pos(), "Argument '$' of function '$' must be '$' not '$'",
					func.parameters[i].name.c_str(), func.name.c_str(), required_type, arg_val->type());
			}
			Environment::Variable arg{ func.parameters[i].name, arg_val->type(), arg_val };
			args.push_back(arg);
		}

		Environment* previous = this->env_;
		this->env_ = new Environment(previous);
		for (Environment::Variable arg : args)
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
			if (!ret.ret_val()->type().is(func.ret_type))
			{
				throw CILError::error(func.body->pos(), "Function '$' should return '$' not '$'",
					func.name, func.ret_type, ret.ret_val()->type());
			}
			return ret.ret_val();
		}
		delete this->env_;
		this->env_ = previous;
		//TODO: Change to none
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

value_ptr Interpreter::run_access_expr(std::shared_ptr<AccessExpression> expr)
{
	Environment::Variable var = env_->get_var(expr->identifier());
	if(!var.type.is_subtype_of(type_id("object")))
	{ throw CILError::error(expr->pos(), "Can only access variables of objects, got '$'", var.type); }
	CIL::Object obj = *std::dynamic_pointer_cast<CIL::Object, CIL::Value>(var.value);

	Environment* previous = this->env_;
	env_ = obj.env();
	env_->add_enclosing(previous);
	value_ptr val = run_expr(expr->inner());
	env_->rem_enclosing();
	this->env_ = previous;
	return val;
}

value_ptr Interpreter::run_new_expr(std::shared_ptr<NewExpression> expr)
{
	Environment::Class cls = env_->get_class(expr->identifier());
	value_ptr obj = CIL::Object::create(cls);
	return obj;
}

value_ptr Interpreter::run_array_access_expr(std::shared_ptr<ArrayAccessExpression> expr)
{
	value_ptr index_num = this->run_expr(expr->index());
	if(!index_num->type().is_subtype_of(type_id("num")))
	{ throw CILError::error(expr->pos(), "Index must be 'num' not '$'", index_num->type()); }
	int index = (int)std::dynamic_pointer_cast<CIL::Number>(index_num)->value();
	Environment::Array arr = this->env_->get_arr(expr->identifier());
	if (index < 0 || index >= arr.size)
	{ throw CILError::error(expr->pos(), "Index must be in the range [$,$[", 0, arr.size); }
	return arr.values[index];
}

value_ptr Interpreter::run_unary_expr(std::shared_ptr<UnaryExpression> expr)
{
	value_ptr inner = this->run_expr(expr->expr());
	if (inner->type().is(type_id("error")))
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
	case Operator::OPERATOR_BITWISE_NOT:
		TRY_OP(return inner->bitwise_not(), expr->pos());
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of unary expressions");
	}
}

value_ptr Interpreter::run_binary_expr(std::shared_ptr<BinaryExpression> expr)
{
	value_ptr left = this->run_expr(expr->left());
	if (left->type().is(type_id("error")))
	{ return CIL::ErrorValue::create(); }
	value_ptr right = this->run_expr(expr->right());
	if (right->type().is(type_id("error")))
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
	case Operator::OPERATOR_BITWISE_AND:
		TRY_OP(return left->bitwise_and(right), expr->pos());
	case Operator::OPERATOR_BITWISE_OR:
		TRY_OP(return left->bitwise_or(right), expr->pos());
	case Operator::OPERATOR_BITWISE_XOR:
		TRY_OP(return left->bitwise_xor(right), expr->pos());
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of binary expressions");
	}
}

value_ptr Interpreter::run_ternary_expr(std::shared_ptr<TernaryExpression> expr)
{
	value_ptr cond = this->run_expr(expr->cond());
	if (cond->type().is(type_id("error")))
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
	if (value->type().is(type_id("error")))
	{ return CIL::ErrorValue::create(); }

	if (expr->target()->is_primary_expr())
	{
		std::shared_ptr<PrimaryExpression> primary = std::dynamic_pointer_cast<PrimaryExpression, Expression>(expr->target());
		if (primary->primary_type() == PrimaryType::PRIMARY_IDENTIFIER)
		{
			std::string identifier = *primary->val().identifier_val;

			if (env_->var_exists(identifier))
			{
				env_->get_var(identifier).value = value;
			}
		}
		else
		{
			throw CILError::error(expr->pos(), "Cannot assign to '$'", primary->primary_type());
		}
	}
	else
	{
		throw CILError::error(expr->pos(), "Cannot only assign to primary values");
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
	case StmtType::STATEMENT_CLASS_DECL:
		run_class_decl_stmt(std::dynamic_pointer_cast<ClassDeclStatement, Statement>(stmt));
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
	std::cout << val->to_string();
}

void Interpreter::run_elif_stmt(std::shared_ptr<ElifStatement> stmt)
{
	if (stmt->cond() == nullptr || run_expr(stmt->cond())->to_bool())
	{
		run_stmt(stmt->inner());
	}
	else if (stmt->next_elif() != nullptr)
	{
		run_elif_stmt(std::dynamic_pointer_cast<ElifStatement, Statement>(stmt->next_elif()));
	}
}

void Interpreter::run_if_stmt(std::shared_ptr<IfStatement> stmt)
{
	value_ptr val = this->run_expr(stmt->cond());
	if (val->to_bool())
	{
		this->run_stmt(stmt->if_branch());
	}
	else if (stmt->top_elif() != nullptr)
	{
		run_stmt(stmt->top_elif());
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

	if (!stmt->info().type.is(value->type()))
	{
		throw CILError::error(stmt->pos(), "Cannot initialize variable of type '$' with value of type '$'",
			stmt->info().type, value->type());
	}
	
	Environment::Variable var{ stmt->info().name, stmt->info().type, value};

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
		if (!val->type().is(stmt->info().type))
		{
			throw CILError::error(stmt->pos(), "Cannot initizalize array of type '$' with value of type '$'",
				stmt->info().type, val->type());
		}
		vals.push_back(val);
	}
	
	Environment::Array arr{ stmt->info().name, stmt->info().size, stmt->info().type, vals};
	this->env_->define_arr(arr);
}

void Interpreter::run_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt)
{
	std::vector<Environment::Variable> args{};
	for (VarInfo arg : stmt->info().args)
	{
		args.push_back({ arg.name, arg.type, nullptr });
	}
	Environment::Function func{ stmt->info().name, stmt->info().ret_type, args, stmt->body() };
	this->env_->define_func(func);
}

void Interpreter::run_class_decl_stmt(std::shared_ptr<ClassDeclStatement> stmt)
{
	std::vector<Environment::Function> methods{};
	std::vector<Environment::Variable> members{};

	for (stmt_ptr s : stmt->methods())
	{
		std::shared_ptr<FuncDeclStatement> method_ptr = std::dynamic_pointer_cast<FuncDeclStatement, Statement>(s);
		std::vector<Environment::Variable> args{};
		for (VarInfo arg : method_ptr->info().args)
		{
			args.push_back({ arg.name, arg.type, nullptr });
		}
		methods.push_back({ method_ptr->info().name, method_ptr->info().ret_type, args, method_ptr->body() });
	}

	for (stmt_ptr s : stmt->members())
	{
		std::shared_ptr<VarDeclStatement> member_ptr = std::dynamic_pointer_cast<VarDeclStatement, Statement>(s);
		value_ptr value = run_expr(member_ptr->val());
		members.push_back({ member_ptr->info().name, member_ptr->info().type, value });
	}

	Environment::Class cls{ stmt->info().name, members, methods };
	env_->define_class(cls);
}

void Interpreter::run_expr_stmt(std::shared_ptr<ExprStatement> stmt)
{
	this->run_expr(stmt->expr());
}

void Interpreter::define_symbols(SymbolTable& table)
{
	for (auto& pair : table.vars_)
	{
		SymbolTable::Variable& var = pair.second;
		value_ptr value = var.init_expr == nullptr ? nullptr : this->run_expr(var.init_expr);
		env_->define_var({ var.name, var.type, value });
	}
	for (auto& pair : table.funcs_)
	{
		SymbolTable::Function& func = pair.second;
		std::vector<Environment::Variable> args{};
		for (SymbolTable::Variable arg : func.args)
		{
			value_ptr value = arg.init_expr == nullptr ? nullptr : this->run_expr(arg.init_expr);
			args.push_back({ arg.name, arg.type, value });
		}
		env_->define_func({ func.name, func.ret_type, args, func.body });
	}
	for (auto& pair : table.classes_)
	{
		SymbolTable::Class& cls = pair.second;
		std::vector<Environment::Function> methods{};
		std::vector<Environment::Variable> members{};

		for (SymbolTable::Function method : cls.methods)
		{
			std::vector<Environment::Variable> args{};
			for (SymbolTable::Variable arg : method.args)
			{
				value_ptr value = arg.init_expr == nullptr ? nullptr : this->run_expr(arg.init_expr);
				args.push_back({ arg.name, arg.type, value });
			}
			methods.push_back({ method.name, method.ret_type, args, method.body });
		}

		for (SymbolTable::Variable member : cls.members)
		{
			value_ptr value = member.init_expr == nullptr ? nullptr : this->run_expr(member.init_expr);
			members.push_back({ member.name, member.type, value });
		}

		env_->define_class({ cls.name, members, methods });
	}
}

void Interpreter::define_global_symbols()
{
	for (auto& pair : SymbolTable::global_table_->vars_)
	{
		SymbolTable::Variable& var = pair.second;
		value_ptr value = var.init_expr == nullptr ? nullptr : this->run_expr(var.init_expr);
		env_->define_var({ var.name, var.type, value });
	}
	for (auto& pair : SymbolTable::global_table_->funcs_)
	{
		SymbolTable::Function& func = pair.second;
		std::vector<Environment::Variable> args{};
		for (SymbolTable::Variable arg : func.args)
		{
			value_ptr value = arg.init_expr == nullptr ? nullptr : this->run_expr(arg.init_expr);
			args.push_back({ arg.name, arg.type, value });
		}
		env_->define_func({ func.name, func.ret_type, args, func.body });
	}
	for (auto& pair : SymbolTable::global_table_->classes_)
	{
		SymbolTable::Class& cls = pair.second;
		std::vector<Environment::Function> methods{};
		std::vector<Environment::Variable> members{};

		for (SymbolTable::Function method : cls.methods)
		{
			std::vector<Environment::Variable> args{};
			for (SymbolTable::Variable arg : method.args)
			{
				value_ptr value = arg.init_expr == nullptr ? nullptr : this->run_expr(arg.init_expr);
				args.push_back({ arg.name, arg.type, value });
			}
			methods.push_back({ method.name, method.ret_type, args, method.body });
		}

		for (SymbolTable::Variable member : cls.members)
		{
			value_ptr value = member.init_expr == nullptr ? nullptr : this->run_expr(member.init_expr);
			members.push_back({ member.name, member.type, value });
		}

		env_->define_class({ cls.name, members, methods });
	}
}
