#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Position.h"

class Token;
typedef std::shared_ptr<Token> token_ptr;
typedef std::vector<token_ptr> token_list;

enum class TokenType
{
	TOKEN_INVALID,
	TOKEN_EOF,
	TOKEN_SYMBOL,
	TOKEN_OPERATOR,
	TOKEN_KEYWORD,
	TOKEN_STRING,
	TOKEN_NUMBER,
	TOKEN_IDENTIFIER
};

enum class Symbol
{
	LEFT_PAREN,
	RIGHT_PAREN,
	LEFT_BRACE,
	RIGHT_BRACE,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	SEMICOLON,
	COMMA,
	DOT,
	COLON,
	QUERY,
	ARROW
};

enum class Operator
{
	OPERATOR_BANG,
	OPERATOR_ADD,
	OPERATOR_INCREMENT,
	OPERATOR_DECREMENT,
	OPERATOR_SUBTRACT,
	OPERATOR_MULTIPLY,
	OPERATOR_DIVIDE,
	OPERATOR_EQUAL,
	OPERATOR_EQUAL_EQUAL,
	OPERATOR_NOT_EQUAL,
	OPERATOR_LESS,
	OPERATOR_GREATER,
	OPERATOR_LESS_EQUAL,
	OPERATOR_GREATER_EQUAL,
	OPERATOR_AND,
	OPERATOR_OR,
	OPERATOR_LEFT_BITSHIFT,
	OPERATOR_RIGHT_BITSHIFT,
	OPERATOR_BITWISE_NOT,
	OPERATOR_BITWISE_AND,
	OPERATOR_BITWISE_OR,
	OPERATOR_BITWISE_XOR
};

enum class Keyword
{
	KEYWORD_AUTO,
	KEYWORD_CONST,
	KEYWORD_TRUE,
	KEYWORD_FALSE,
	KEYWORD_PRINT,
	KEYWORD_IF,
	KEYWORD_ELIF,
	KEYWORD_ELSE,
	KEYWORD_FOR,
	KEYWORD_WHILE,
	KEYWORD_BREAK,
	KEYWORD_DEF,
	KEYWORD_RETURN,
	KEYWORD_CLASS,
	KEYWORD_NEW,
	KEYWORD_THIS,
	KEYWORD_EXTENDS
};

class Token
{
public:
	Token(const token_ptr&);
	Token& operator=(const token_ptr&);

	TokenType type() const;
	Position pos() const;
	const std::string& lexeme() const;

	bool is_invalid() const;
	bool is_EOF() const;
	bool is_symbol() const;
	bool is_operator() const;
	bool is_keyword() const;
	bool is_string() const;
	bool is_number() const;
	bool is_identifier() const;

	Symbol symbol() const;
	Operator op() const;
	Keyword keyword() const;
	const std::string& identifier() const;
	const std::string& string_val() const;
	const double number_val() const;

	static token_ptr create_invalid_token(Position pos);
	static token_ptr create_eof_token(Position pos);
	static token_ptr create_symbol_token(Symbol type, std::string lexeme, Position pos);
	static token_ptr create_operator_token(Operator type, std::string lexeme, Position pos);
	static token_ptr create_keyword_token(Keyword type, std::string lexeme, Position pos);
	static token_ptr create_string_token(const std::string& text, std::string lexeme, Position pos);
	static token_ptr create_number_token(double value, std::string lexeme, Position pos);
	static token_ptr create_identifier_token(const std::string& value, std::string lexeme, Position pos);

	const std::string to_string() const;
private:
	Token(TokenType type, std::string lexeme, Position pos);

	TokenType type_;

	std::string lexeme_;

	union
	{
		std::string* string_value;
		double number_value;
		Symbol symbol_value;
		Operator operator_value;
		Keyword keyword_value;
	} value_;

	Position pos_;
};