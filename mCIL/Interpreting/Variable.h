#pragma once
#include "../cil-system.h"
#include "Object.h"

struct VarInfo
{
	std::string name;
	//TODO: Seperate into type-struct
	ObjType type;
	bool is_const;
};

struct Variable
{
	VarInfo info;
	Object value;
};