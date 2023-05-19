#pragma once
#include "../cil-system.h"
#include "../Types/Type.h"

struct VarInfo
{
	std::string name;
	Type type;
};

struct FuncInfo
{
	std::string name;
	std::vector<VarInfo> args;
	Type ret_type;
	bool has_return;
};

struct ArrInfo
{
	std::string name;
	Type type;
	int size;
};

struct ClassInfo
{
	std::string name;
	std::vector<FuncInfo> methods;
	std::vector<VarInfo> members;
};