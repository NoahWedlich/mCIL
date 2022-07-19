#pragma once
#include "../cil-system.h"
#include "../Lexing/Token.h"
#include "Expression.h"

class Parser
{
public:
	Parser(std::vector<Token>& tokens);

	program_t& parse();
private:
	Token peek()
	{ return this->tokens_[this->current]; }
	Token advance()
	{ return this->tokens_[this->current++]; }
	Token consume()
	{ return this->tokens_[++this->current]; }
	bool atEnd()
	{ return this->peek().is_EOF(); }

	bool match_number();
	bool match_string();
	bool match_identifier();
	bool match_symbol(Symbol sym);
	bool match_operator(Operator op);
	bool match_operators(Operator a, Operator b)
	{ return this->match_operator(a) || this->match_operator(b); }
	bool match_keyword(Keyword key);

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

	std::vector<Token>& tokens_;
	int current;
};

