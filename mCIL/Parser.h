#pragma once
#include "cil-system.h"
#include "Token.h"
#include "Expression.h"

class Parser
{
public:
	Parser(std::vector<Token>& tokens);

	std::vector<Expression>& parse();
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

	Expression* grouping_expr();
	Expression* primary_expr();
	Expression* unary_expr();
	Expression* factor_expr();
	Expression* sum_expr();
	Expression* comparison_expr();
	Expression* equality_expr();
	Expression* logical_expr();
	Expression* ternary_expr();
	Expression* assignment_expr();
	Expression* expression();

	std::vector<Token>& tokens_;
	int current;
};

