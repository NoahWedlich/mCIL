#include "Diagnostics.h"

bool ErrorManager::error_ocurred = false;
std::vector<cil_error_t> ErrorManager::errors{};

void ErrorManager::cil_error(const Position& pos, const std::string& msg)
{
	ErrorManager::errors.push_back(cil_error_t{ pos, msg });
	ErrorManager::error_ocurred = true;
}

void ErrorManager::cil_warning(const Position& pos, const std::string& msg)
{
	std::cout << "Warning:\n\t" << pos.line << ":" << pos.startChar << "\t" << msg << "\n" << std::endl;
}

void ErrorManager::report_errors(SourceManager& source)
{
	for (cil_error_t error : ErrorManager::errors)
	{
		std::string line_prefix = std::to_string(error.first.line + 1) + " | ";
		size_t prefix_len = line_prefix.size();
		std::string carot_prefix = std::string(prefix_len + error.first.startChar, ' ');
		std::cout << "Error:\t" << error.second << "\n\n";
		std::cout << "\t" << line_prefix << source.get_line_at_off(error.first.line) << "\n";
		std::cout << "\t" << carot_prefix << "^" << std::endl;
	}
}

void ErrorManager::clear_errors()
{
	ErrorManager::errors.clear();
	ErrorManager::error_ocurred = false;
}