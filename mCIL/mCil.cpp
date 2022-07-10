#include "cil-system.h"
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "SourceFileManager.h"
#include "Diagnostics.h"

int main()
{
	SourceFileManager source{ "ParseTest.cil" };
	Lexer lexer{ source };
	std::vector<Token> tokens = lexer.scan_file();
	if (ErrorManager::error_ocurred)
	{
		ErrorManager::report_errors(source);
		exit(EXIT_FAILURE);
	}
	Parser parser{ tokens };
	std::vector<Expression> exprs = *parser.parse();
	int test = 0;
}