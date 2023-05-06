#include "cil-system.h"
#include "Lexing/Lexer.h"
#include "Parsing/Parser.h"
#include "Parsing/Statement.h"
#include "Diagnostics/SourceFileManager.h"
#include "Diagnostics/Diagnostics.h"
#include "Tools/ASTDebugPrinter.h"
#include "Interpreting/Interpreter.h"
#include "REPL/REPL.h"

#include "Utils/Threading/FunctionJob.h"
#include "Utils/Threading/JobResult.h"
#include "Utils/Threading/Worker.h"
#include "Utils/Threading/ThreadManager.h"

int test(int a, int b)
{ return a + b; }

int main()
{
	ThreadManager manager{ 2 };
	auto a = manager.schedule_func(std::function<int(int, int)>(test), 1, 2);
	auto b = manager.schedule_func(std::function<int(int, int)>(test), 1, 2);
	auto c = manager.schedule_func(std::function<int(int, int)>(test), 1, 2);
	auto d = manager.schedule_func(std::function<int(int, int)>(test), 1, 2);

	manager.wait_all();

	exit(0);

	/*REPL repl{};
	repl.run();*/

	SourceFileManager source{ "Samples/None.cil" };
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

	ASTDebugPrinter dbg{};
	dbg.print_stmt_list(stmts);
	
	Interpreter interpreter{ stmts };
	interpreter.run();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}
}