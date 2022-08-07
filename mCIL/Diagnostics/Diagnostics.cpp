#include "Diagnostics.h"

bool ErrorManager::error_ocurred = false;
std::vector<CILError> ErrorManager::errors{};

void ErrorManager::cil_error(const CILError& err)
{
	ErrorManager::errors.push_back(err);
	ErrorManager::error_ocurred = true;
}

void ErrorManager::cil_warning(const Position& pos, const std::string& msg)
{
	std::cout << "Warning:\n\t" << pos.start_pos().line_off << ":" << pos.start_pos().char_off << "\t" << msg << "\n" << std::endl;
}

void ErrorManager::report_errors(SourceManager& source)
{
	for (const CILError& error : ErrorManager::errors)
	{
		std::string line_prefix = std::to_string(error.range().start_pos().line_off + 1) + " | ";
		size_t prefix_len = line_prefix.size();
		std::string carot_prefix = std::string(prefix_len + error.focus().start_pos().char_off, ' ');
		std::string underline = std::string(error.focus().end_pos().char_off - error.focus().start_pos().char_off + 1, '^');
		std::string source_line = source.get_line_at_off(error.range().start_pos().line_off);

		std::cout << "\nError: " << error.what() << "\n\n";
		std::cout << "\t" << line_prefix << source_line << (source_line.ends_with("\n") ? "" : "\n");
		std::cout << "\t" << carot_prefix << underline << std::endl;
	}
}

void ErrorManager::clear_errors()
{
	ErrorManager::errors.clear();
	ErrorManager::error_ocurred = false;
}