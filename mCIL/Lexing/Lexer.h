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

	token_list scan_file();
private:
	token_ptr next_token();

	bool char_is_alpha(char);

	int read_line();
	bool read_required_line();
	Position pos(size_t len);

	token_ptr create_invalid_token();
	token_ptr create_eof_token();
	token_ptr create_keyword_token(Keyword type, std::string lexeme, size_t len = 1);
	token_ptr create_string_token(const std::string& text, std::string lexeme, size_t len = 1);
	token_ptr create_number_token(double value, std::string lexeme, size_t len = 1);
	token_ptr create_identifier_token(const std::string& value, std::string lexeme, size_t len = 1);
	token_ptr create_operator_token(Operator type, std::string lexeme, size_t len = 1);
	token_ptr create_symbol_token(Symbol type, std::string lexeme, size_t len = 1);

	bool skip_spaces();
	bool skip_comments();

	token_ptr get_symbol(bool& found);
	token_ptr get_operator(bool& found);
	token_ptr get_number(bool &found);
	token_ptr get_string(bool& found);
	token_ptr get_keyword(bool& found);
	token_ptr get_identifier(bool& found);

	SourceManager& source_;
	char* current_line_;
	size_t max_line_size_;
	size_t cur_line_size_;
	size_t char_off_;
	size_t line_off_;

	size_t start_char_;

	static std::map<std::string, Keyword> keyword_map;
};