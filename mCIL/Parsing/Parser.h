#pragma once
#include "../cil-system.h"
#include "../Types/Type.h"
#include "../Types/TypeTable.h"
#include "../Lexing/Token.h"
#include "../Diagnostics/CILError.h"
#include "../Diagnostics/Diagnostics.h"
#include "Expression.h"
#include "Statement.h"
#include "../Scanning/SymbolTable.h"

class Parser
{
public:
	Parser(token_list& tokens);

	stmt_list& parse();

	expr_ptr parse_single_expr();
	stmt_ptr parse_single_stmt();

	void reset();
private:
	token_ptr peek()
	{ return this->tokens_[this->current]; }
	token_ptr advance()
	{ return this->tokens_[this->current++]; }
	token_ptr consume()
	{ return this->tokens_[++this->current]; }
	bool atEnd()
	{ return this->peek()->is_EOF(); }

	void expect_number();
	void expect_str();
	void expect_identifier();
	void expect_symbol(Symbol symbol);
	void expect_op(Operator op);
	void expect_keyword(Keyword keyword);

	void synchronize();

	Position pos_from_tokens(token_ptr start, token_ptr end);
	Position pos_from_exprs(expr_ptr start, expr_ptr end);
	Position pos_from_stmts(stmt_ptr start, stmt_ptr end);

	bool match_number();
	bool match_string();
	bool match_identifier();
	bool match_symbol(Symbol sym);
	bool match_operator(Operator op);
	bool match_operators(Operator a, Operator b)
	{ return this->match_operator(a) || this->match_operator(b); }
	bool match_keyword(Keyword key);
	
	bool match_type();
	bool get_type(Type& type);

	expr_ptr grouping_expr();
	expr_ptr primary_expr();
	expr_ptr call_expr();
	expr_ptr access_expr();
	expr_ptr new_expr();
	expr_ptr array_access_expr();
	expr_ptr unary_expr();
	expr_ptr factor_expr();
	expr_ptr sum_expr();
	expr_ptr bitshift_expr();
	expr_ptr comparison_expr();
	expr_ptr equality_expr();
	expr_ptr bitwise_expr();
	expr_ptr logical_and_expr();
	expr_ptr logical_or_expr();
	expr_ptr ternary_expr();
	expr_ptr assignment_expr();
	expr_ptr expression();

	stmt_ptr expr_stmt();
	stmt_ptr block_stmt();
	stmt_ptr break_stmt();
	stmt_ptr return_stmt();
	stmt_ptr print_stmt();
	stmt_ptr elif_stmt();
	stmt_ptr if_stmt();
	stmt_ptr while_stmt();
	stmt_ptr for_stmt();
	stmt_ptr var_decl_stmt();
	stmt_ptr arr_decl_stmt();
	stmt_ptr func_decl_stmt();
	stmt_ptr class_decl_stmt();
	stmt_ptr statement();

	void parse_variable(SymbolTable::Variable& var);
	void parse_function(SymbolTable::Function& func);
	void parse_class(SymbolTable::Class& cls);

	token_list& tokens_;
	int current;

	int func_level;
	int loop_level;

	bool has_return_;
};

