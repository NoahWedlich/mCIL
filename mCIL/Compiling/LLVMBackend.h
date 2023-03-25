#pragma once
#include "../cil-system.h"
#include "LLVMHeaders.h"
#include "Backend.h"
#include "../Parsing/Expression.h"
#include "../Parsing/Statement.h"
#include "../Diagnostics/CILError.h"
#include "../Diagnostics/Diagnostics.h"

typedef llvm::Value* val;

class LLVMBackend : public Backend
{
public:
	LLVMBackend();
	virtual void init() override;
	virtual void gen_statement(stmt_ptr stmt) override;
private:
	val gen_expr(expr_ptr expr);

	val gen_grouping_expr(std::shared_ptr<GroupingExpression> expr);
	val gen_primary_expr(std::shared_ptr<PrimaryExpression> expr);
	val gen_call_expr(std::shared_ptr<CallExpression> expr);
	val gen_access_expr(std::shared_ptr<AccessExpression> expr);
	val gen_new_expr(std::shared_ptr<NewExpression> expr);
	val gen_array_access_expr(std::shared_ptr<ArrayAccessExpression> expr);
	val gen_unary_expr(std::shared_ptr<UnaryExpression> expr);
	val gen_binary_expr(std::shared_ptr<BinaryExpression> expr);
	val gen_ternary_expr(std::shared_ptr<TernaryExpression> expr);
	val gen_assignment_expr(std::shared_ptr<AssignmentExpression> expr);

	val gen_stmt(stmt_ptr stmt);

	val gen_block_stmt(std::shared_ptr<BlockStatement> stmt);
	val gen_break_stmt(std::shared_ptr<BreakStatement> stmt);
	val gen_return_stmt(std::shared_ptr<ReturnStatement> stmt);
	val gen_print_stmt(std::shared_ptr<PrintStatement> stmt);
	val gen_else_stmt(std::shared_ptr<ElseStatement> stmt);
	val gen_elif_stmt(std::shared_ptr<ElifStatement> stmt);
	val gen_if_stmt(std::shared_ptr<IfStatement> stmt);
	val gen_while_stmt(std::shared_ptr<WhileStatement> stmt);
	val gen_for_stmt(std::shared_ptr<ForStatement> stmt);
	val gen_var_decl_stmt(std::shared_ptr<VarDeclStatement> stmt);
	val gen_arr_decl_stmt(std::shared_ptr<ArrDeclStatement> stmt);
	val gen_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt);
	val gen_class_decl_stmt(std::shared_ptr<ClassDeclStatement> stmt);
	val gen_expr_stmt(std::shared_ptr<ExprStatement> stmt);

	std::unique_ptr<llvm::LLVMContext> context_;
	std::unique_ptr<llvm::IRBuilder<>> builder_;
	std::unique_ptr<llvm::Module> module_;
	std::map<std::string, llvm::Value*> named_values_;

	template <typename T>
	CILError unsupported(Position pos, T op);
};

template<typename T>
inline CILError LLVMBackend::unsupported(Position pos, T op)
{
	return CILError::error(pos, "Compilation of '$' is not supported", op);
}
