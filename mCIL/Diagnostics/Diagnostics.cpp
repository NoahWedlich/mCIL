#include "Diagnostics.h"

bool ErrorManager::error_ocurred = false;
std::vector<cil_error_t> ErrorManager::errors{};

void ErrorManager::cil_error(const Position& pos, const std::string& msg, const std::string& prefix)
{
	ErrorManager::errors.push_back(cil_error_t{ pos, msg, prefix });
	ErrorManager::error_ocurred = true;
}

void ErrorManager::cil_lexer_error(const LexerError& err)
{
	ErrorManager::cil_error(err.pos, err.what(), "LexerError");
}

void ErrorManager::cil_parser_error(const ParserError& err)
{
	ErrorManager::cil_error(err.pos, err.what(), "ParserError");
}

void ErrorManager::cil_interpreter_error(const InterpreterError& err)
{
	ErrorManager::cil_error(err.pos, err.what(), "InterpreterError");
}

void ErrorManager::cil_warning(const Position& pos, const std::string& msg)
{
	std::cout << "Warning:\n\t" << pos.line << ":" << pos.startChar << "\t" << msg << "\n" << std::endl;
}

void ErrorManager::report_errors(SourceManager& source)
{
	for (const cil_error_t& error : ErrorManager::errors)
	{
		std::string line_prefix = std::to_string(error.pos.line + 1) + " | ";
		size_t prefix_len = line_prefix.size();
		std::string carot_prefix = std::string(prefix_len + error.pos.startChar, ' ');
		std::string source_line = source.get_line_at_off(error.pos.line);
		std::cout << "\n" + error.prefix + ": " << error.msg << "\n\n";
		std::cout << "\t" << line_prefix << source_line << (source_line.ends_with("\n") ? "" : "\n");
		std::cout << "\t" << carot_prefix << "^" << std::endl;
	}
}

void ErrorManager::clear_errors()
{
	ErrorManager::errors.clear();
	ErrorManager::error_ocurred = false;
}