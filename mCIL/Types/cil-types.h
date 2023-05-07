#pragma once
#include "../cil-system.h"

enum Type
{
	NONE,
	BOOL,
	NUM,
	STR,
	OBJ,
	UNKNOWN,
	ERROR
};

struct cilType
{
	cilType()
		: type(Type::UNKNOWN), is_const(false) {}
	cilType(Type type)
		: type(type), is_const(false) {}
	cilType(Type type, bool is_const)
		: type(type), is_const(is_const) {}

	Type type;
	bool is_const;
};

struct VarInfo
{
	std::string name;
	cilType type;
};

struct FuncInfo
{
	std::string name;
	std::vector<VarInfo> args;
	cilType ret_type;
	bool has_return;
};

struct ArrInfo
{
	std::string name;
	cilType type;
	int size;
};

struct ClassInfo
{
	std::string name;
	std::vector<FuncInfo> methods;
	std::vector<VarInfo> members;
};

bool operator==(cilType a, cilType b);

bool operator!=(cilType a, cilType b);