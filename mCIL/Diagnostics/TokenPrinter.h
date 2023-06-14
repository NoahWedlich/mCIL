#pragma once

#include "../cil-system.h"
#include "../Lexing/Token.h"

namespace TokenPrinter
{
	std::string print_symbol(Symbol s);

	std::string print_operator(Operator op);

	std::string print_keyword(Keyword k);

	std::string print_token(token_ptr t);

	std::string print_token_type(TokenType type);
}