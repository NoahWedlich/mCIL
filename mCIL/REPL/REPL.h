#pragma once
#include "../cil-system.h"
#include "REPLFileManager.h"
#include "../Diagnostics/Diagnostics.h"
#include "../Lexing/Lexer.h"
#include "../Parsing/Parser.h"
#include "../Parsing/Expression.h"
#include "../Parsing/Statement.h"
#include "../Interpreting/Interpreter.h"

class REPL
{
public:
	REPL();

	void run();
private:
	Lexer lexer_;
	REPLFileManager source_;
};

