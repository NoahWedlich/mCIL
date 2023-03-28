#include "cil-system.h"
#include "Lexing/Lexer.h"
#include "Parsing/Parser.h"
#include "Parsing/Statement.h"
#include "Compiling/Compiler.h"
#include "Compiling/LLVMBackend.h"
#include "Diagnostics/SourceFileManager.h"
#include "Diagnostics/Diagnostics.h"
#include "Tools/ASTDebugPrinter.h"
#include "Interpreting/Interpreter.h"
#include "REPL/REPL.h"

int main()
{
	/*REPL repl{};
	repl.run();*/

	SourceFileManager source{ "Samples/Functions.cil" };
	Lexer lexer{ source };
	std::vector<Token> tokens = lexer.scan_file();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}

	Parser parser{ tokens };
	stmt_list& stmts = parser.parse();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}

	/*ASTDebugPrinter dbg{};
	dbg.print_stmt_list(stmts);*/

	Compiler compiler{ stmts, std::shared_ptr<Backend>(new LLVMBackend())};
	compiler.compile();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}
	
	/*Interpreter interpreter{ stmts };
	interpreter.run();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}*/
}