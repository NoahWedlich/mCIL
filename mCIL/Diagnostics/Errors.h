#pragma once
#include "../cil-system.h"
#include "../Parsing/Expression.h"

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

class LexerError : public CIL_Error
{
public:
	LexerError(const std::string& msg, Position pos)
		: CIL_Error(msg), pos_(pos) {}

	Position pos_;
};

class ParserError : public CIL_Error
{
public:
	ParserError(const std::string& msg, Expression& expr)
		: CIL_Error(msg), expr_(expr) {}

	Expression expr_;
};

class InterpreterError : public CIL_Error
{
public:
	InterpreterError(const std::string& msg, Expression& expr)
		: CIL_Error(msg), expr_(expr) {}

	Expression expr_;
};
