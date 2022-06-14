#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include "Token.h"

class Lexer
{
public:
	Lexer(const char* filepath);
	~Lexer();

	std::vector<Token> scan();
	void dumpTokens();
private:
	std::ifstream file;
	std::vector<Token> tokens;
	size_t line;
	size_t curChar;

	static std::map<std::string, Symbol> symbolMap;
	static std::map<std::string, Operator> operatorMap;
	static std::map<std::string, Keyword> keywordMap;
};

