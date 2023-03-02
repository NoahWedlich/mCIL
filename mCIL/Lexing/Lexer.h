#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Diagnostics.h"
#include "../Diagnostics/Position.h"
#include "../Diagnostics/SourceManager.h"
#include "../Diagnostics/CILError.h"
#include "Token.h"

class Lexer
{
public:
	Lexer(SourceManager& source);
	~Lexer();

	std::vector<Token> scan_file();
private:
	Token next_token();

	bool char_is_alpha(char);

	int read_line();
	bool read_required_line();
	Position pos(size_t len);

	Token create_invalid_token();
	Token create_eof_token();
	Token create_keyword_token(Keyword type, std::string lexeme, size_t len = 1);
	Token create_string_token(const std::string& text, std::string lexeme, size_t len = 1);
	Token create_number_token(double value, std::string lexeme, size_t len = 1);
	Token create_identifier_token(const std::string& value, std::string lexeme, size_t len = 1);
	Token create_operator_token(Operator type, std::string lexeme, size_t len = 1);
	Token create_symbol_token(Symbol type, std::string lexeme, size_t len = 1);

	bool skip_spaces();
	bool skip_comments();

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

	size_t start_char_;

	static std::map<std::string, Keyword> keyword_map;
};