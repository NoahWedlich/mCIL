#pragma once
#include "../cil-system.h"
#include "../Parsing/Statement.h"

class Backend
{
public:
	virtual void init() = 0;

	virtual void gen_statement(stmt_ptr stmt) = 0;
};