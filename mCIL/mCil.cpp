#include "cil-system.h"
#include "Lexing/Lexer.h"
#include "Parsing/Parser.h"
#include "Diagnostics/SourceFileManager.h"
#include "Diagnostics/Diagnostics.h"
#include "Tools/ASTDebugPrinter.h"

int main()
{
	SourceFileManager source{ "Samples/ParseTest.cil" };
	Lexer lexer{ source };
	std::vector<Token> tokens = lexer.scan_file();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}
	Parser parser{ tokens };

	//TODO: Change to statements
	std::vector<Expression> exprs = *parser.parse();
	
	ASTDebugPrinter dbg{ std::cout };
	dbg.print_expression(&exprs[0]);
}