#pragma once
#include "cil-system.h"
#include "SourceManager.h"
#include "Position.h"

typedef std::pair<const Position, const std::string> cil_error_t;

struct ErrorManager
{
	static void cil_error(const Position& pos, const std::string& msg);

	static void cil_warning(const Position& pos, const std::string& msg);

	static void report_errors(SourceManager& source);

	static bool error_ocurred;
	static std::vector<cil_error_t> errors;
};