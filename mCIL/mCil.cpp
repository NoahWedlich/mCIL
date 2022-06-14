#include <iostream>
#include "Lexer.h"

int main()
{
	Lexer lexer{ "Sample.cil" };
	lexer.scan();
	lexer.dumpTokens();
}