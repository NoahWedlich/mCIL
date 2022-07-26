#pragma once
#include "../cil-system.h"
#include "SourceManager.h"
#include "Position.h"
#include "Errors.h"

struct cil_error_t
{
	const Position pos;
	const std::string msg;
	const std::string prefix;
};

struct ErrorManager
{
	static void cil_error(const Position& pos, const std::string& msg, const std::string& prefix = "Error");

	static void cil_lexer_error(const LexerError& err);
	static void cil_parser_error(const ParserError& err);
	static void cil_interpreter_error(const InterpreterError& err);

	static void cil_warning(const Position& pos, const std::string& msg);

	static void report_errors(SourceManager& source);

	static void clear_errors();

	static bool error_ocurred;
	static std::vector<cil_error_t> errors;
};