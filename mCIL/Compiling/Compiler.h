#pragma once
#include "../cil-system.h"
#include "Backend.h"
#include "../Parsing/Expression.h"
#include "../Parsing/Statement.h"
#include "../Diagnostics/CILError.h"
#include "../Diagnostics/Diagnostics.h"

class Compiler
{
public:
	Compiler(std::shared_ptr<Backend> backend);
	Compiler(stmt_list& program, std::shared_ptr<Backend> backend);

	~Compiler();

	void compile();

	void compile_single_statement(stmt_ptr stmt);
private:
	stmt_list program_;

	std::shared_ptr<Backend> backend_;
};

