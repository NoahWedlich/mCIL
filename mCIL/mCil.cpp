#include "cil-system.h"
#include "Lexing/Lexer.h"
#include "Parsing/Parser.h"
#include "Parsing/Statement.h"
#include "Parsing/Declaration.h"
#include "Diagnostics/SourceFileManager.h"
#include "Diagnostics/Diagnostics.h"
#include "Tools/ASTDebugPrinter.h"
#include "Tools/ASTPrettyPrinter.h"
#include "Interpreting/Interpreter.h"
#include "REPL/REPL.h"

int main()
{
	//REPL repl{};
	//repl.run();

	SourceFileManager source{ "Samples/Variables.cil" };
	Lexer lexer{ source };
	std::vector<Token> tokens = lexer.scan_file();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}
	Parser parser{ tokens };

	decl_list& decls = parser.parse();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}

	/*ASTDebugPrinter dbg{ std::cout };
	dbg.print_expression(exprs[0]);*/
	
	/*Interpreter interpreter{ decls };
	interpreter.run();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}*/
}