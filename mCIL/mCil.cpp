#include <iostream>
#include "Lexer.h"
#include "Token.h"

int main()
{
	std::ifstream file{ "LexerTest.cil" };
	Lexer lexer{ "LexerTest", file };
	std::vector<Token> tokens = lexer.scan_file();
	for (Token& token : tokens)
	{
		std::cout << token.toString() << std::endl;
	}
}