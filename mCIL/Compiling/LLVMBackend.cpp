#include "LLVMBackend.h"

LLVMBackend::LLVMBackend()
	: context_(nullptr), builder_(nullptr), module_(nullptr), named_values_()
{
}

void LLVMBackend::init()
{
	context_ = std::make_unique<llvm::LLVMContext>();
	module_ = std::make_unique<llvm::Module>("mCIL", *context_);

	builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
}

void LLVMBackend::gen_statement(stmt_ptr stmt)
{
	val IR = gen_stmt(stmt);
	IR->print(llvm::errs());
	llvm::errs() << "\n";
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
		throw unsupported(expr->pos(), PrimaryType::PRIMARY_IDENTIFIER);
	default:
		throw CILError::error(expr->pos(), "Incomplete handling of primary expressions");
	}
}

val LLVMBackend::gen_call_expr(std::shared_ptr<CallExpression> expr)
{
	throw unsupported(expr->pos(), ExprType::EXPRESSION_CALL);
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
	throw unsupported(expr->pos(), ExprType::EXPRESSION_BINARY);
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
	case StmtType::STATEMENT_ELSE:
		return gen_else_stmt(std::dynamic_pointer_cast<ElseStatement, Statement>(stmt));
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
	throw unsupported(stmt->pos(), StmtType::STATEMENT_BLOCK);
}

val LLVMBackend::gen_break_stmt(std::shared_ptr<BreakStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_BREAK);
}

val LLVMBackend::gen_return_stmt(std::shared_ptr<ReturnStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_RETURN);
}

val LLVMBackend::gen_print_stmt(std::shared_ptr<PrintStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_PRINT);
}

val LLVMBackend::gen_else_stmt(std::shared_ptr<ElseStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_ELSE);
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

val LLVMBackend::gen_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt)
{
	throw unsupported(stmt->pos(), StmtType::STATEMENT_FUNC_DECL);
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
