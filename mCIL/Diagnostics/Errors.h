#pragma once
#include "../cil-system.h"
#include "../Parsing/Statement.h"
#include "../Parsing/Expression.h"
#include "../Parsing/Declaration.h"

class CIL_Error : public std::exception
{
public:
	CIL_Error(const std::string& msg)
		: msg_(msg) {}

	const char* what() const
	{
		return this->msg_.c_str();
	}

private:
	const std::string msg_;
};

struct LexerError : public CIL_Error
{
	LexerError(const std::string& msg, Position pos)
		: CIL_Error(msg), pos(pos) {}

	Position pos;
};

struct ParserError : public CIL_Error
{
	ParserError(const std::string& msg, Token token)
		: CIL_Error(msg), pos(token.position()) {}

	ParserError(const std::string& msg, Statement stmt)
		: CIL_Error(msg), pos(stmt.pos()) {}

	ParserError(const std::string& msg, Expression expr)
		: CIL_Error(msg), pos(expr.pos()) {}

	ParserError(const std::string& msg, Declaration decl)
		: CIL_Error(msg), pos(decl.pos()) {}

	Position pos;
};

struct InterpreterError : public CIL_Error
{
	InterpreterError(const std::string& msg, Statement& stmt)
		: CIL_Error(msg), pos(stmt.pos()) {}

	InterpreterError(const std::string& msg, Expression expr)
		: CIL_Error(msg), pos(expr.pos()) {}

	Position pos;
};
