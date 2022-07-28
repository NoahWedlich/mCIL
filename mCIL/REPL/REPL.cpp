#include "REPL.h"

REPL::REPL()
	: source_(), lexer_(source_) {}

void REPL::run()
{
	std::cout << "mCIL's REPL:\n";

	bool run = true;
	while (run)
	{
		std::cout << " > ";
		std::string buffer;
		std::getline(std::cin, buffer);
		this->source_.add_line(buffer);

		std::vector<Token> tokens = this->lexer_.scan_file();
		if (ErrorManager::error_ocurred)
		{
			ErrorManager::report_errors(this->source_);
			ErrorManager::clear_errors();
			this->source_.clear();
			continue;
		}

		Parser parser{ tokens };
		stmt_list& program = parser.parse();
		if (ErrorManager::error_ocurred)
		{
			ErrorManager::report_errors(this->source_);
			ErrorManager::clear_errors();
			this->source_.clear();
			continue;
		}

		//TODO: Default constructor
		Interpreter interpreter{ program };
		//TODO: Statement
		//Object result = interpreter.run_single_expression(program[0]);
		Object result = Object::create_error_object();
		if (ErrorManager::error_ocurred)
		{
			ErrorManager::report_errors(this->source_);
			ErrorManager::clear_errors();
			this->source_.clear();
			continue;
		}

		std::cout << result.to_string() << std::endl;
		this->source_.clear();
	}
}
