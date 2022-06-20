#include "cil-system.h"
#include "Lexer.h"
#include "Token.h"
#include "SourceFileManager.h"
#include "Diagnostics.h"

int main()
{
	SourceFileManager source{ "LexerTest.cil" };
	Lexer lexer{ source };
	std::vector<Token> tokens = lexer.scan_file();
	if (ErrorManager::error_ocurred)
	{
		exit(EXIT_FAILURE);
	}
	for (Token& token : tokens)
	{
		std::cout << token.to_string() << std::endl;
	}
}