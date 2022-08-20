#pragma once
#include "../cil-system.h"
#include "../Lexing/Token.h"
#include "../Diagnostics/CILError.h"
#include "../Diagnostics/Diagnostics.h"
#include "Expression.h"
#include "Statement.h"

class Parser
{
public:
	Parser(std::vector<Token>& tokens);

	stmt_list& parse();

	expr_ptr parse_single_expr();
	stmt_ptr parse_single_stmt();

	void reset();
private:
	Token peek()
	{ return this->tokens_[this->current]; }
	Token advance()
	{ return this->tokens_[this->current++]; }
	Token consume()
	{ return this->tokens_[++this->current]; }
	bool atEnd()
	{ return this->peek().is_EOF(); }

	Position pos_from_tokens(Token start, Token end);
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
	
	bool get_type(ObjType& type, bool& is_const);

	void consume_semicolon(Position pos);

	expr_ptr grouping_expr();
	expr_ptr primary_expr();
	expr_ptr unary_expr();
	expr_ptr factor_expr();
	expr_ptr sum_expr();
	expr_ptr comparison_expr();
	expr_ptr equality_expr();
	expr_ptr logical_and_expr();
	expr_ptr logical_or_expr();
	expr_ptr ternary_expr();
	expr_ptr assignment_expr();
	expr_ptr expression();

	stmt_ptr expr_stmt();
	stmt_ptr block_stmt();
	stmt_ptr return_stmt();
	stmt_ptr print_stmt();
	stmt_ptr if_stmt();
	stmt_ptr while_stmt();
	stmt_ptr for_stmt();
	stmt_ptr var_decl_stmt();
	stmt_ptr func_decl_stmt();
	stmt_ptr statement();

	std::vector<Token>& tokens_;
	int current;

	int func_level;
};

