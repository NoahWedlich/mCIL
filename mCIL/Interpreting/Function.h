#pragma once
#include "../cil-system.h"
#include "Variable.h"

class Statement;
typedef std::shared_ptr<Statement> stmt_ptr;

struct FuncInfo
{
	std::string name;
	std::vector<VarInfo> args;
	ObjType ret_type;
	bool ret_is_const;
};

struct Function
{
	FuncInfo info;
	stmt_ptr body;
};