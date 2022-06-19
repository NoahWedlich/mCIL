#pragma once
#include "cil-system.h"
#include "Position.h"

struct ErrorManager
{
	static void cil_error(const Position& pos, const std::string& msg);

	static void cil_warning(const Position& pos, const std::string& msg);

	static bool error_ocurred;
};