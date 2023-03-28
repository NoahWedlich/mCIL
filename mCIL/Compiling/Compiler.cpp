#include "Compiler.h"

Compiler::Compiler(std::shared_ptr<Backend> backend)
	: program_(), backend_(backend)
{
	backend_->init();
}

Compiler::Compiler(stmt_list& program, std::shared_ptr<Backend> backend)
	: program_(program), backend_(backend)
{
	backend_->init();
}

Compiler::~Compiler()
{
}

void Compiler::compile()
{
	for (stmt_ptr stmt : program_)
	{
		try
		{
			backend_->gen_statement(stmt);
		}
		catch (CILError& err)
		{
			if (!err.has_pos())
			{ err.add_range(stmt->pos()); }
			ErrorManager::cil_error(err);
			break;
		}
	}

	backend_->dump();
}

void Compiler::compile_single_statement(stmt_ptr stmt)
{
	try
	{
		backend_->gen_statement(stmt);
	}
	catch (CILError& err)
	{
		if (!err.has_pos())
		{ err.add_range(stmt->pos()); }
		ErrorManager::cil_error(err);
	}
}
