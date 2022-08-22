#pragma once
#include "cil-types.h"
#include "../Interpreting/Object.h"

class Statement;
typedef std::shared_ptr<Statement> stmt_ptr;

struct Variable
{
	VarInfo info;
	Object value;
};

struct Function
{
	FuncInfo info;
	stmt_ptr body;
};