#pragma once
#include "../cil-system.h"
#include "../Types/cil-types.h"
#include "../Types/BuiltinTypes.h"
#include "Environment.h"
#include "../Parsing/Expression.h"
#include "../Parsing/Statement.h"
#include "../Diagnostics/CILError.h"
#include "../Diagnostics/Diagnostics.h"

#define TRY_OP(op, pos) \
try                     \
{                       \
	op;                 \
}                       \
catch(CILError& err)    \
{                       \
	err.add_range(pos); \
	throw err;          \
}                       \

class Return : public std::exception
{
public:
	Return(value_ptr ret_val)
		: ret_val_(ret_val) {}

	value_ptr ret_val()
	{ return this->ret_val_; }
private:
	value_ptr ret_val_;
};

class Break : public std::exception
{
};

class Interpreter
{
public:
	Interpreter();
	Interpreter(stmt_list& program);

	~Interpreter();

	void run();

	void run_single_statement(stmt_ptr stmt);
	value_ptr run_single_expression(expr_ptr expr);

private:
	value_ptr run_expr(expr_ptr expr);

	value_ptr run_grouping_expr(std::shared_ptr<GroupingExpression> expr);
	value_ptr run_primary_expr(std::shared_ptr<PrimaryExpression> expr);
	value_ptr run_call_expr(std::shared_ptr<CallExpression> expr);
	value_ptr run_array_access_expr(std::shared_ptr<ArrayAccessExpression> expr);
	value_ptr run_unary_expr(std::shared_ptr<UnaryExpression> expr);
	value_ptr run_binary_expr(std::shared_ptr<BinaryExpression> expr);
	value_ptr run_ternary_expr(std::shared_ptr<TernaryExpression> expr);
	value_ptr run_assignment_expr(std::shared_ptr<AssignmentExpression> expr);

	void run_stmt(stmt_ptr stmt);

	void run_block_stmt(std::shared_ptr<BlockStatement> stmt);
	void run_break_stmt(std::shared_ptr<BreakStatement> stmt);
	void run_return_stmt(std::shared_ptr<ReturnStatement> stmt);
	void run_print_stmt(std::shared_ptr<PrintStatement> stmt);
	void run_if_stmt(std::shared_ptr<IfStatement> stmt);
	void run_while_stmt(std::shared_ptr<WhileStatement> stmt);
	void run_for_stmt(std::shared_ptr<ForStatement> stmt);
	void run_var_decl_stmt(std::shared_ptr<VarDeclStatement> stmt);
	void run_arr_decl_stmt(std::shared_ptr<ArrDeclStatement> stmt);
	void run_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt);
	void run_class_decl_stmt(std::shared_ptr<ClassDeclStatement> stmt);
	void run_expr_stmt(std::shared_ptr<ExprStatement> stmt);

	stmt_list program_;

	Environment* env_;
};

