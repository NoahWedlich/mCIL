#pragma once
#include "../cil-system.h"
#include "../Types/Type.h"
#include "../Types/TypeTable.h"
#include "../Lexing/Token.h"
#include "../Diagnostics/CILError.h"
#include "SymbolTable.h"

class Scanner
{
public:
	Scanner(token_list& code);

	token_list scan();
private:
	token_list scan_stmt();
	token_list scan_block();

	token_ptr peek();
	token_ptr advance();
	token_ptr consume();
	bool at_end();

	//TODO: This is simply copied from the parser, should be moved to a common place
	token_ptr expect_number();
	token_ptr expect_string();
	token_ptr expect_identifier();
	token_ptr expect_symbol(Symbol symbol);
	token_ptr expect_operator(Operator op);
	token_ptr expect_keyword(Keyword keyword);
	Type expect_type();

	bool match_number(bool consume = true);
	bool match_string(bool consume = true);
	bool match_identifier(bool consume = true);
	bool match_symbol(Symbol symbol, bool consume = true);
	bool match_operator(Operator op, bool consume = true);
	bool match_keyword(Keyword keyword, bool consume = true);
	bool match_type(bool consume = true);

	bool scan_variable();
	bool scan_function();
	bool scan_class();

	token_list& code_;
	SymbolTable table_;

	int current_;
};