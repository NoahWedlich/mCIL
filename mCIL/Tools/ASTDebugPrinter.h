#pragma once
#include "../cil-system.h"
#include "../Types/Type.h"
#include "../Types/TypeTable.h"
#include "../Diagnostics/TokenPrinter.h"
#include "../Diagnostics/CILError.h"
#include "../Parsing/Expression.h"
#include "../Parsing/Statement.h"

#define InitRepr()						\
	std::string result = "";			\
	result += std::string(level_, '\t');

#define ReprEqual(stmt) \
	result += stmt;

#define ReprChild(stmt) \
	level_++;		\
	result += stmt;	\
	level_--;

class ASTDebugPrinter
{
public:
	ASTDebugPrinter(std::ostream& os = std::cout)
		: os_(os), level_(0) {}

	void print_expression(expr_ptr expr);
	void print_statement(stmt_ptr stmt);
	void print_expr_list(expr_list exprs);
	void print_stmt_list(stmt_list stmts);

private:
	std::ostream& os_;

	size_t level_;

	std::string repr_cil_type(Type type);

	std::string repr_expr(expr_ptr expr);

	std::string repr_grouping_expr(std::shared_ptr<GroupingExpression> expr);
	std::string repr_primary_expr(std::shared_ptr<PrimaryExpression> expr);
	std::string repr_call_expr(std::shared_ptr<CallExpression> expr);
	std::string repr_access_expr(std::shared_ptr<AccessExpression> expr);
	std::string repr_new_expr(std::shared_ptr<NewExpression> expr);
	std::string repr_array_access_expr(std::shared_ptr<ArrayAccessExpression> expr);
	std::string repr_unary_expr(std::shared_ptr<UnaryExpression> expr);
	std::string repr_binary_expr(std::shared_ptr<BinaryExpression> expr);
	std::string repr_ternary_expr(std::shared_ptr<TernaryExpression> expr);
	std::string repr_assignment_expr(std::shared_ptr<AssignmentExpression> expr);

	std::string repr_stmt(stmt_ptr stmt);

	std::string repr_block_stmt(std::shared_ptr<BlockStatement> stmt);
	std::string repr_break_stmt(std::shared_ptr<BreakStatement> stmt);
	std::string repr_return_stmt(std::shared_ptr<ReturnStatement> stmt);
	std::string repr_print_stmt(std::shared_ptr<PrintStatement> stmt);
	std::string repr_elif_stmt(std::shared_ptr<ElifStatement> stmt);
	std::string repr_if_stmt(std::shared_ptr<IfStatement> stmt);
	std::string repr_while_stmt(std::shared_ptr<WhileStatement> stmt);
	std::string repr_for_stmt(std::shared_ptr<ForStatement> stmt);
	std::string repr_var_decl_stmt(std::shared_ptr<VarDeclStatement> stmt);
	std::string repr_arr_decl_stmt(std::shared_ptr<ArrDeclStatement> stmt);
	std::string repr_func_decl_stmt(std::shared_ptr<FuncDeclStatement> stmt);
	std::string repr_class_decl_stmt(std::shared_ptr<ClassDeclStatement> stmt);
	std::string repr_expr_stmt(std::shared_ptr<ExprStatement> stmt);
};

