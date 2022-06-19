#include "Diagnostics.h"

bool ErrorManager::error_ocurred = false;

void ErrorManager::cil_error(const Position& pos, const std::string& msg)
{
	std::cout << "Error:\n\t" << pos.line << ":" << pos.startChar << "\t" << msg << "\n" << std::endl;
	ErrorManager::error_ocurred = true;
}

void ErrorManager::cil_warning(const Position& pos, const std::string& msg)
{
	std::cout << "Warning:\n\t" << pos.line << ":" << pos.startChar << "\t" << msg << "\n" << std::endl;
}
