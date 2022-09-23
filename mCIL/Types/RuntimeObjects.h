#pragma once
#include "cil-types.h"
#include "Value.h"

class Statement;
typedef std::shared_ptr<Statement> stmt_ptr;

struct Variable
{
	VarInfo info;
	value_ptr value;
};

struct Function
{
	FuncInfo info;
	stmt_ptr body;
};

struct Array
{
	ArrInfo info;
	std::vector<value_ptr> arr;
};

struct Class
{
	ClassInfo info;
	std::vector<Function> methods;
	std::vector<Variable> members;
};