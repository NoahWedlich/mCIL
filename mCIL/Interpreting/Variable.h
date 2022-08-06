#pragma once
#include "../cil-system.h"
#include "Object.h"

struct Variable
{
	std::string name;
	ObjType type;
	bool is_const;
	Object value;
};