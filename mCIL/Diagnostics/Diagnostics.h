#pragma once
#include "../cil-system.h"
#include "SourceManager.h"
#include "Position.h"
#include "CILError.h"

struct ErrorManager
{
	static void cil_error(const CILError& err);

	//TODO: Add Warnings
	static void cil_warning(const Position& pos, const std::string& msg);

	static void report_errors(SourceManager& source);

	static void clear_errors();

	static bool error_ocurred;
	static std::vector<CILError> errors;
};