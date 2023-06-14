#include "REPL.h"

REPL::REPL()
	: source_(), lexer_(source_) {}

void REPL::run()
{
	std::cout << "mCIL's REPL:\n";

	Interpreter interpreter{};

	bool run = true;
	while (run)
	{
		std::cout << " > ";
		std::string buffer;
		std::getline(std::cin, buffer);
		this->source_.add_line(buffer);

		token_list tokens = this->lexer_.scan_file();
		if (ErrorManager::error_ocurred)
		{
			ErrorManager::report_errors(this->source_);
			ErrorManager::clear_errors();
			this->source_.clear();
			continue;
		}

		Parser parser{ tokens };

		expr_ptr expr = parser.parse_single_expr();
		if (!expr->is_error_expr())
		{
			value_ptr value = interpreter.run_single_expression(expr);
			if (!value->type().is(type_id("error")))
			{
				std::cout << value->to_string() << std::endl;
				continue;
			}
		}
		else
		{
			parser.reset();
			ErrorManager::clear_errors();
			stmt_ptr stmt = parser.parse_single_stmt();
			if (!stmt->is_error_stmt())
			{
				interpreter.run_single_statement(stmt);
			}
		}

		if (ErrorManager::error_ocurred)
		{
			ErrorManager::report_errors(this->source_);
			ErrorManager::clear_errors();
			this->source_.clear();
			continue;
		}

		this->source_.clear();
	}
}
