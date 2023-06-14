#include "cil-system.h"
#include "Types/TypeTable.h"
#include "Lexing/Lexer.h"
#include "Scanning/Scanner.h"
#include "Parsing/Parser.h"
#include "Parsing/Statement.h"
#include "Compiling/Compiler.h"
#include "Compiling/LLVMBackend.h"
#include "Diagnostics/SourceFileManager.h"
#include "Diagnostics/Diagnostics.h"
#include "Utils/Debugging/ASTDebugPrinter.h"
#include "Interpreting/Interpreter.h"
#include "REPL/REPL.h"

int main()
{
	/*REPL repl{};
	repl.run();*/

	TypeTable::add_builtin_types();

	SourceFileManager source{ "Samples/Functions.cil" };
	Lexer lexer{ source };
	token_list tokens = lexer.scan_file();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}

	Scanner scanner{ tokens };
	token_list top_level_tokens = scanner.scan();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}

	Parser parser{ top_level_tokens };
	stmt_list& stmts = parser.parse();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}

	ASTDebugPrinter dbg{};
	dbg.print_stmt_list(stmts);

	/*Compiler compiler{ stmts, std::shared_ptr<Backend>(new LLVMBackend())};
	compiler.compile();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}*/
	
	Interpreter interpreter{ stmts };
	interpreter.run();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}
}