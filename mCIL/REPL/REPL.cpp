#include "REPL.h"

REPL::REPL()
	: source_(), lexer_(source_) {}

void REPL::run()
{
	//TODO: Make istream and ostream changeable
	std::cout << "mCIL's REPL:\n";

	bool run = true;
	while (run)
	{
		std::cout << " > ";
		std::string buffer;
		std::getline(std::cin, buffer);
		this->source_.add_line(buffer);

		try
		{
			std::vector<Token> tokens = this->lexer_.scan_file();
			if (ErrorManager::error_ocurred)
			{
				ErrorManager::report_errors(this->source_);
				ErrorManager::clear_errors();
			}

			Parser parser{ tokens };
			program_t& program = parser.parse();
			//TODO: Default constructor
			Interpreter interpreter{ program };
			Object result = interpreter.run_single_expression(program[0]);
			std::cout << result.to_string() << std::endl;
		}
		catch (std::invalid_argument)
		{
			//TODO: Error reporting
			std::cout << "Error" << std::endl;
		}
		this->source_.clear();
	}
}
