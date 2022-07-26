#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Diagnostics.h"
#include "../Diagnostics/Position.h"
#include "../Diagnostics/SourceManager.h"
#include "../Diagnostics/Errors.h"
#include "Token.h"

class Lexer
{
public:
	Lexer(SourceManager& source);
	~Lexer();

	std::vector<Token> scan_file();
private:
	Token next_token();

	int read_line();
	bool read_required_line();
	Position position(int offset = 0);

	Token create_invalid_token();
	Token create_eof_token();
	Token create_keyword_token(Keyword type, std::string lexeme);
	Token create_string_token(const std::string& text, std::string lexeme);
	Token create_number_token(double value, std::string lexeme);
	Token create_identifier_token(const std::string& value, std::string lexeme);
	Token create_operator_token(Operator type, std::string lexeme);
	Token create_symbol_token(Symbol type, std::string lexeme);

	void  skip_spaces();

	Token get_symbol(bool& found);
	Token get_operator(bool& found);
	Token get_number(bool &found);
	Token get_string(bool& found);
	Token get_keyword(bool& found);
	Token get_identifier(bool& found);

	SourceManager& source_;
	char* current_line_;
	size_t max_line_size_;
	size_t cur_line_size_;
	size_t char_off_;
	size_t line_off_;


	static std::map<std::string, Keyword> keyword_map;
};