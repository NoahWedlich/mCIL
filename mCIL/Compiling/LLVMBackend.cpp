#include "LLVMBackend.h"

LLVMBackend::LLVMBackend()
	: context_(nullptr), builder_(nullptr), module_(nullptr), named_values_(), entry_(nullptr)
{
}

void LLVMBackend::init()
{
	context_ = std::make_unique<llvm::LLVMContext>();
	module_ = std::make_unique<llvm::Module>("mCIL", *context_);

	builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);

	llvm::FunctionType* main_func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), false);
	llvm::Function* main_func = llvm::Function::Create(main_func_type, llvm::Function::ExternalLinkage, "internal_main", module_.get());

	entry_ = llvm::BasicBlock::Create(*context_, "entry", main_func);
}

void LLVMBackend::gen_statement(stmt_ptr stmt)
{
	builder_->SetInsertPoint(entry_);
	val IR = gen_stmt(stmt);
	//IR->print(llvm::errs());
	//llvm::errs() << "\n";
}

void LLVMBackend::dump()
{
	module_->dump();
}

val LLVMBackend::gen_expr(expr_ptr expr)
{
	switch (expr->type())
	{
	case ExprType::EXPRESSION_ERROR:
		throw unsupported(expr->pos(), PrimaryType::PRIMARY_NONE);
	case ExprType::EXPRESSION_GROUPING:
		return gen_grouping_expr(std::dynamic_pointer_cast<GroupingExpression, Expression>(expr));
	case ExprType::EXPRESSION_CALL:
		return gen_call_expr(std::dynamic_pointer_cast<CallExpression, Expression>(expr));
	case ExprType::EXPRESSION_ACCESS:
		return gen_access_expr(std::dynamic_pointer_cast<AccessExpression, Expression>(expr));
	case ExprType::EXPRESSION_NEW:
		return gen_new_expr(std::dynamic_pointer_cast<NewExpression, Expression>(expr));
	case ExprType::EXPRESSION_ARRAY_ACCESS:
		return gen_array_access_expr(std::dynamic_pointer_cast<ArrayAccessExpression, Expression>(expr));
	case ExprType::EXPRESSION_PRIMARY:
		return gen_primary_expr(std::dynamic_pointer_cast<PrimaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_UNARY:
		return gen_unary_expr(std::dynamic_pointer_cast<UnaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_BINARY:
		return gen_binary_expr(std::dynamic_pointer_cast<BinaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_TERNARY:
		return gen_ternary_expr(std::dynamic_pointer_cast<TernaryExpression, Expression>(expr));
	case ExprType::EXPRESSION_ASSIGNMENT:
		return gen_assignment_expr(std::dynamic_pointer_cast<AssignmentExpression, Expression>(expr));
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of expressions");
	}
}

val LLVMBackend::gen_grouping_expr(std::shared_ptr<GroupingExpression> expr)
{
	return gen_expr(expr->expr());
}

val LLVMBackend::gen_primary_expr(std::shared_ptr<PrimaryExpression> expr)
{
	switch (expr->primary_type())
	{
	case PrimaryType::PRIMARY_NONE:
		throw unsupported(expr->pos(), PrimaryType::PRIMARY_NONE);
	case PrimaryType::PRIMARY_BOOL:
		return builder_->getInt1(expr->val().bool_val);
	case PrimaryType::PRIMARY_NUM:
		return llvm::ConstantFP::get(*context_, llvm::APFloat(expr->val().num_val));
	case PrimaryType::PRIMARY_STR:
		return llvm::ConstantDataArray::getString(*context_, *expr->val().str_val);
	case PrimaryType::PRIMARY_IDENTIFIER:
	{
		val var = named_values_[*expr->val().str_val];
		if (!var)
		{
			throw CILError::error(expr->pos(), "Unknown variable: '$'", *expr->val().str_val);
		}
		return var;
	}
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of primary expressions");
	}
}

val LLVMBackend::gen_call_expr(std::shared_ptr<CallExpression> expr)
{
	llvm::Function* func = module_->getFunction(expr->identifier());
	if (!func)
	{
		throw CILError::error(expr->pos(), "Unknown functon '$'", expr->identifier());
	}

	if (expr->args().size() != func->arg_size())
	{
		throw CILError::error(expr->pos(), "Function '$' expects $ arguments, got $", expr->identifier(), func->arg_size(), expr->args().size());
	}

	std::vector<val> args;
	for (int i = 0; i < func->arg_size(); i++)
	{
		args.push_back(gen_expr(expr->args()[i]));
	}

	return builder_->CreateCall(func, args);
}

val LLVMBackend::gen_access_expr(std::shared_ptr<AccessExpression> expr)
{
	throw unsupported(expr->pos(), ExprType::EXPRESSION_ACCESS);
}

val LLVMBackend::gen_new_expr(std::shared_ptr<NewExpression> expr)
{
	throw unsupported(expr->pos(), ExprType::EXPRESSION_NEW);
}

val LLVMBackend::gen_array_access_expr(std::shared_ptr<ArrayAccessExpression> expr)
{
	throw unsupported(expr->pos(), ExprType::EXPRESSION_ARRAY_ACCESS);
}

val LLVMBackend::gen_unary_expr(std::shared_ptr<UnaryExpression> expr)
{
	val inner = gen_expr(expr->expr());

	switch (expr->op())
	{
	case Operator::OPERATOR_BANG:
		if (is_num(inner))
		{
			return builder_->CreateFCmpOEQ(inner, llvm::ConstantFP::get(*context_, llvm::APFloat(0.0)), "UnaryNot");
		}
		else if (is_bool(inner))
		{
			return builder_->CreateICmpEQ(inner, llvm::ConstantInt::get(*context_, llvm::APInt(1, 0)), "UnaryNot");
		}
		else
		{
			throw invalid_unary(expr->pos(), Operator::OPERATOR_BANG, inner);
		}
	case Operator::OPERATOR_SUBTRACT:
		if (is_num(inner))
		{
			return builder_->CreateFMul(llvm::ConstantFP::get(*context_, llvm::APFloat(-1.0)), inner, "UnaryInvert");
		}
		else
		{
			throw invalid_unary(expr->pos(), Operator::OPERATOR_SUBTRACT, inner);
		}
	case Operator::OPERATOR_INCREMENT:
		if (is_num(inner))
		{
			return builder_->CreateFAdd(inner, llvm::ConstantFP::get(*context_, llvm::APFloat(1.0)), "UnaryIncrement");
		}
		else
		{
			throw invalid_unary(expr->pos(), Operator::OPERATOR_INCREMENT, inner);
		}
	case Operator::OPERATOR_DECREMENT:
		if (is_num(inner))
		{
			return builder_->CreateFSub(inner, llvm::ConstantFP::get(*context_, llvm::APFloat(1.0)), "UnaryDecrement");
		}
		else
		{
			throw invalid_unary(expr->pos(), Operator::OPERATOR_DECREMENT, inner);
		}
	case Operator::OPERATOR_BITWISE_NOT:
		throw unsupported(expr->pos(), Operator::OPERATOR_BITWISE_NOT);
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of unary operators");
	}
}

val LLVMBackend::gen_binary_expr(std::shared_ptr<BinaryExpression> expr)
{
	val left = gen_expr(expr->left());
	val right = gen_expr(expr->right());

	switch (expr->op())
	{
	case Operator::OPERATOR_ADD:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFAdd(left, right, "NumAdd");
		}
		//TODO: Custom str-class
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_ADD, left, right);
		}
	case Operator::OPERATOR_SUBTRACT:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFSub(left, right, "NumSub");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_SUBTRACT, left, right);
		}
	case Operator::OPERATOR_MULTIPLY:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFMul(left, right, "NumMult");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_MULTIPLY, left, right);
		}
	case Operator::OPERATOR_DIVIDE:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFDiv(left, right, "NumDiv");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_DIVIDE, left, right);
		}
	case Operator::OPERATOR_LEFT_BITSHIFT:
		throw unsupported(expr->pos(), Operator::OPERATOR_LEFT_BITSHIFT);
	case Operator::OPERATOR_RIGHT_BITSHIFT:
		throw unsupported(expr->pos(), Operator::OPERATOR_RIGHT_BITSHIFT);
	case Operator::OPERATOR_GREATER:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFCmpOGT(left, right, "NumGreater");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_GREATER, left, right);
		}
	case Operator::OPERATOR_LESS:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFCmpOLT(left, right, "NumLess");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_LESS, left, right);
		}
	case Operator::OPERATOR_GREATER_EQUAL:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFCmpOGE(left, right, "NumGreaterEqual");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_GREATER_EQUAL, left, right);
		}
	case Operator::OPERATOR_LESS_EQUAL:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFCmpOLE(left, right, "NumLessEqual");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_LESS_EQUAL, left, right);
		}
	case Operator::OPERATOR_EQUAL_EQUAL:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFCmpOEQ(left, right, "NumEquals");
		}
		else if (is_bool(left) && is_bool(right))
		{
			return builder_->CreateICmpEQ(left, right, "BoolEquals");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_EQUAL_EQUAL, left, right);
		}
	case Operator::OPERATOR_NOT_EQUAL:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateFCmpONE(left, right, "NumNotEquals");
		}
		else if (is_bool(left) && is_bool(right))
		{
			return builder_->CreateICmpNE(left, right, "BoolNotEquals");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_NOT_EQUAL, left, right);
		}
	case Operator::OPERATOR_AND:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateLogicalAnd(left, right, "NumAnd");
		}
		else if (is_bool(left) && is_bool(right))
		{
			return builder_->CreateLogicalAnd(left, right, "BoolAnd");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_AND, left, right);
		}
	case Operator::OPERATOR_OR:
		if (is_num(left) && is_num(right))
		{
			return builder_->CreateLogicalOr(left, right, "NumOr");
		}
		else if (is_bool(left) && is_bool(right))
		{
			return builder_->CreateLogicalOr(left, right, "BoolOr");
		}
		else
		{
			throw invalid_binary(expr->pos(), Operator::OPERATOR_OR, left, right);
	case Operator::OPERATOR_BITWISE_AND:
		throw unsupported(expr->pos(), Operator::OPERATOR_BITWISE_AND);
	case Operator::OPERATOR_BITWISE_OR:
		throw unsupported(expr->pos(), Operator::OPERATOR_BITWISE_OR);
	case Operator::OPERATOR_BITWISE_XOR:
		throw unsupported(expr->pos(), Operator::OPERATOR_BITWISE_XOR);
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of binary expressions");
		}
	}
}

val LLVMBackend::gen_ternary_expr(std::shared_ptr<TernaryExpression> expr)
{
	throw unsupported(expr->pos(), ExprType::EXPRESSION_TERNARY);
}

val LLVMBackend::gen_assignment_expr(std::shared_ptr<AssignmentExpression> expr)
{
	throw unsupported(expr->pos(), ExprType::EXPRESSION_ASSIGNMENT);
}

val LLVMBackend::gen_stmt(stmt_ptr stmt)
{
	switch (stmt->type())
	{
	case StmtType::STATEMENT_BLOCK:
		return gen_block_stmt(std::dynamic_pointer_cast<BlockStatement, Statement>(stmt));
	case StmtType::STATEMENT_BREAK:
		return gen_break_stmt(std::dynamic_pointer_cast<BreakStatement, Statement>(stmt));
	case StmtType::STATEMENT_RETURN:
		return gen_return_stmt(std::dynamic_pointer_cast<ReturnStatement, Statement>(stmt));
	case StmtType::STATEMENT_PRINT:
		return gen_print_stmt(std::dynamic_pointer_cast<PrintStatement, Statement>(stmt));
	case StmtType::STATEMENT_ELIF:
		return gen_elif_stmt(std::dynamic_pointer_cast<ElifStatement, Statement>(stmt));
	case StmtType::STATEMENT_IF:
		return gen_if_stmt(std::dynamic_pointer_cast<IfStatement, Statement>(stmt));
	case StmtType::STATEMENT_WHILE:
		return gen_while_stmt(std::dynamic_pointer_cast<WhileStatement, Statement>(stmt));
	case StmtType::STATEMENT_FOR:
		return gen_for_stmt(std::dynamic_pointer_cast<ForStatement, Statement>(stmt));
	case StmtType::STATEMENT_VAR_DECL:
		return gen_var_decl_stmt(std::dynamic_pointer_cast<VarDeclStatement, Statement>(stmt));
	case StmtType::STATEMENT_ARR_DECL:
		return gen_arr_decl_stmt(std::dynamic_pointer_cast<ArrDeclStatement, Statement>(stmt));
	case StmtType::STATEMENT_FUNC_DECL:
		return gen_func_decl_stmt(std::dynamic_pointer_cast<FuncDeclStatement, Statement>(stmt));
	case StmtType::STATEMENT_CLASS_DECL:
		return gen_class_decl_stmt(std::dynamic_pointer_cast<ClassDeclStatement, Statement>(stmt));
	case StmtType::STATEMENT_EXPR:
		return gen_expr_stmt(std::dynamic_pointer_cast<ExprStatement, Statement>(stmt));
	default:
		throw CILError::error(stmt->pos(), "Incomplete handling of statements");
	}
}

val LLVMBackend::gen_block_stmt(std::shared_ptr<BlockStatement> stmt)
{
	//TODO: Change this maybe?
	val last_stmt = nullptr;
	for (stmt_ptr inner : stmt->inner())
	{
		last_stmt = gen_stmt(inner);
	}
	return last_stmt;
}

val LLVMBackend::gen_break_stmt(std::shared_ptr<BreakStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_BREAK);
}

val LLVMBackend::gen_return_stmt(std::shared_ptr<ReturnStatement> stmt)
{
	val ret_val = gen_expr(stmt->expr());
	return builder_->CreateRet(ret_val);
}

val LLVMBackend::gen_print_stmt(std::shared_ptr<PrintStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_PRINT);
}

val LLVMBackend::gen_elif_stmt(std::shared_ptr<ElifStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_ELIF);
}

val LLVMBackend::gen_if_stmt(std::shared_ptr<IfStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_IF);
}

val LLVMBackend::gen_while_stmt(std::shared_ptr<WhileStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_WHILE);
}

val LLVMBackend::gen_for_stmt(std::shared_ptr<ForStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_FOR);
}

val LLVMBackend::gen_var_decl_stmt(std::shared_ptr<VarDeclStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_VAR_DECL);
}

val LLVMBackend::gen_arr_decl_stmt(std::shared_ptr<ArrDeclStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_ARR_DECL);
}

llvm::Function* LLVMBackend::gen_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt)
{
	llvm::Function* function = module_->getFunction(stmt->info().name);

	if (!function)
	{
		std::vector<llvm::Type*> arg_types{};
		for (VarInfo info : stmt->info().args)
		{
			arg_types.push_back(cilType_to_LLVM_Type(info.type));
		}

		llvm::FunctionType* func_type = llvm::FunctionType::get(cilType_to_LLVM_Type(stmt->info().ret_type), arg_types, false);
		function = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, stmt->info().name, module_.get());

		unsigned index = 0;
		for (auto& arg : function->args())
		{
			arg.setName(stmt->info().args[index++].name);
		}
	}

	if (!function)
	{
		throw CILError::error("Error creating function '$'", stmt->info().name);
	}
	else
	{
		if (stmt->body())
		{
			if (!function->empty())
			{
				throw CILError::error(stmt->pos(), "Function '$' cannot be redefined", stmt->info().name);
			}

			llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context_, "entry", function);
			builder_->SetInsertPoint(entry);

			for (auto& arg : function->args())
			{
				named_values_[std::string(arg.getName())] = &arg;
			}

			val body = gen_stmt(stmt->body());
			builder_->CreateRetVoid();

			llvm::verifyFunction(*function);
		}

		return function;
	}
}

val LLVMBackend::gen_class_decl_stmt(std::shared_ptr<ClassDeclStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_CLASS_DECL);
}

val LLVMBackend::gen_expr_stmt(std::shared_ptr<ExprStatement> stmt)
{
	return gen_expr(stmt->expr());
}

bool LLVMBackend::is_num(val value)
{
	llvm::Type* type = value->getType();
	return type->isDoubleTy();
}

bool LLVMBackend::is_str(val value)
{
	llvm::Type* type = value->getType();
	return type->isArrayTy();
}

bool LLVMBackend::is_bool(val value)
{
	llvm::Type* type = value->getType();
	return type->isIntegerTy(1);
}

llvm::Type* LLVMBackend::cilType_to_LLVM_Type(cilType type)
{
	switch (type.type)
	{
		case Type::NONE:
			return llvm::Type::getVoidTy(*context_); //TODO: Change this to custom none
		case Type::BOOL:
			return llvm::Type::getInt1Ty(*context_);
		case Type::NUM:
			return llvm::Type::getDoubleTy(*context_);
		case Type::STR:
			throw CILError::error("Cannot compile 'str'");
		case Type::OBJ:
			throw CILError::error("Cannot compile 'obj'");
		case Type::UNKNOWN:
			throw CILError::error("Cannot compile 'unknown'");
		case Type::ERROR:
			throw CILError::error("Cannot compile 'error'");
	}
}
