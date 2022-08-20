#pragma once
#include "../cil-system.h"
#include "Object.h"
#include "Environment.h"
#include "Variable.h"
#include "Function.h"
#include "../Parsing/Expression.h"
#include "../Parsing/Statement.h"
#include "../Diagnostics/CILError.h"
#include "../Diagnostics/Diagnostics.h"

class Return : public std::exception
{
public:
	Return(Object ret_val)
		: ret_val_(ret_val) {}

	Object ret_val()
	{ return this->ret_val_; }
private:
	Object ret_val_;
};

class Interpreter
{
public:
	Interpreter();
	Interpreter(stmt_list& program);

	void run();

	void run_single_statement(stmt_ptr stmt);
	Object run_single_expression(expr_ptr expr);

private:
	void assert_binary_types(Operator op, Object left, Object right, ObjType left_t, ObjType right_t, Position pos);

	Object run_expr(expr_ptr expr);

	Object run_grouping_expr(std::shared_ptr<GroupingExpression> expr);
	Object run_primary_expr(std::shared_ptr<PrimaryExpression> expr);
	Object run_unary_expr(std::shared_ptr<UnaryExpression> expr);
	Object run_binary_expr(std::shared_ptr<BinaryExpression> expr);
	Object run_ternary_expr(std::shared_ptr<TernaryExpression> expr);
	Object run_assignment_expr(std::shared_ptr<AssignmentExpression> expr);

	void run_stmt(stmt_ptr stmt);

	void run_block_stmt(std::shared_ptr<BlockStatement> stmt);
	void run_return_stmt(std::shared_ptr<ReturnStatement> stmt);
	void run_print_stmt(std::shared_ptr<PrintStatement> stmt);
	void run_if_stmt(std::shared_ptr<IfStatement> stmt);
	void run_while_stmt(std::shared_ptr<WhileStatement> stmt);
	void run_for_stmt(std::shared_ptr<ForStatement> stmt);
	void run_var_decl_stmt(std::shared_ptr<VarDeclStatement> stmt);
	void run_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt);
	void run_expr_stmt(std::shared_ptr<ExprStatement> stmt);

	stmt_list program_;

	Environment env_;
};

