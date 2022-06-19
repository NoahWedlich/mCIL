#include "cil-system.h"
#include "Lexer.h"
#include "Token.h"
#include "Diagnostics.h"

int main()
{
	std::ifstream file{ "LexerTest.cil" };
	Lexer lexer{ "LexerTest", file };
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