#include "Token.h"

Token::Token(TokenType type, std::string lexeme, Position pos)
	: type_(type), lexeme_(lexeme), pos_(pos), value_() {}

Token::Token(const Token& other)
	: type_(other.type_), lexeme_(other.lexeme_), pos_(other.pos_), value_()
{
	switch (this->type_)
	{
	case TokenType::TOKEN_INVALID:
	case TokenType::TOKEN_EOF:
		break;
	case TokenType::TOKEN_KEYWORD:
		this->value_.keyword_value = other.value_.keyword_value;
		break;
	case TokenType::TOKEN_IDENTIFIER:
	case TokenType::TOKEN_STRING:
		this->value_.string_value = other.value_.string_value;
		break;
	case TokenType::TOKEN_OPERATOR:
		this->value_.operator_value = other.value_.operator_value;
		break;
	case TokenType::TOKEN_SYMBOL:
		this->value_.symbol_value = other.value_.symbol_value;
		break;
	case TokenType::TOKEN_NUMBER:
		this->value_.number_value = other.value_.number_value;
		break;
	default:
		assert(false);
	}
}

Token& Token::operator=(const Token& other)
{
	this->type_ = other.type_;
	this->lexeme_ = other.lexeme_;
	this->pos_ = other.pos_;
	switch (other.type_)
	{
	case TokenType::TOKEN_INVALID:
	case TokenType::TOKEN_EOF:
		break;
	case TokenType::TOKEN_KEYWORD:
		this->value_.keyword_value = other.value_.keyword_value;
		break;
	case TokenType::TOKEN_IDENTIFIER:
	case TokenType::TOKEN_STRING:
		this->value_.string_value = other.value_.string_value;
		break;
	case TokenType::TOKEN_OPERATOR:
		this->value_.operator_value = other.value_.operator_value;
		break;
	case TokenType::TOKEN_SYMBOL:
		this->value_.symbol_value = other.value_.symbol_value;
		break;
	case TokenType::TOKEN_NUMBER:
		this->value_.number_value = other.value_.number_value;
	}
	return *this;
}

TokenType Token::type() const
{
	return this->type_;
}

Position Token::position() const
{
	return this->pos_;
}

const std::string& Token::lexeme() const
{
	return this->lexeme_;
}

bool Token::isInvalid() const
{
	return this->type_ == TokenType::TOKEN_INVALID;
}

bool Token::isEOF() const
{
	return this->type_ == TokenType::TOKEN_EOF;
}

bool Token::isSymbol() const
{
	return this->type_ == TokenType::TOKEN_SYMBOL;
}

bool Token::isOperator() const
{
	return this->type_ == TokenType::TOKEN_OPERATOR;
}

bool Token::isKeyword() const
{
	return this->type_ == TokenType::TOKEN_KEYWORD;
}

bool Token::isString() const
{
	return this->type_ == TokenType::TOKEN_STRING;
}

bool Token::isNumber() const
{
	return this->type_ == TokenType::TOKEN_NUMBER;
}

bool Token::isIdentifier() const
{
	return this->type_ == TokenType::TOKEN_IDENTIFIER;
}

Symbol Token::symbol() const
{
	assert(this->type_ == TokenType::TOKEN_SYMBOL);
	return this->value_.symbol_value;
}

Operator Token::op() const
{
	assert(this->type_ == TokenType::TOKEN_OPERATOR);
	return this->value_.operator_value;
}

Keyword Token::keyword() const
{
	assert(this->type_ == TokenType::TOKEN_KEYWORD);
	return this->value_.keyword_value;
}

const std::string& Token::identifier() const
{
	assert(this->type_ == TokenType::TOKEN_IDENTIFIER);
	return *this->value_.string_value;
}

const std::string& Token::string_val() const
{
	assert(this->type_ == TokenType::TOKEN_STRING);
	return *this->value_.string_value;
}

const double Token::number_val() const
{
	assert(this->type_ == TokenType::TOKEN_NUMBER);
	return this->value_.number_value;
}

Token Token::create_invalid_token(Position pos)
{
	return Token(TokenType::TOKEN_INVALID, "", pos);
}

Token Token::create_eof_token(Position pos)
{
	return Token(TokenType::TOKEN_EOF, "", pos);
}

Token Token::create_symbol_token(Symbol type, std::string lexeme, Position pos)
{
	Token token(TokenType::TOKEN_SYMBOL, lexeme, pos);
	token.value_.symbol_value = type;
	return token;
}

Token Token::create_operator_token(Operator type, std::string lexeme, Position pos)
{
	Token token(TokenType::TOKEN_OPERATOR, lexeme, pos);
	token.value_.operator_value = type;
	return token;
}

Token Token::create_keyword_token(Keyword type, std::string lexeme, Position pos)
{
	Token token(TokenType::TOKEN_KEYWORD, lexeme, pos);
	token.value_.keyword_value = type;
	return token;
}

Token Token::create_string_token(const std::string& text, std::string lexeme, Position pos)
{
	Token token(TokenType::TOKEN_STRING, lexeme, pos);
	token.value_.string_value = new std::string(text);
	return token;
}

Token Token::create_number_token(double value, std::string lexeme, Position pos)
{
	Token token(TokenType::TOKEN_NUMBER, lexeme, pos);
	token.value_.number_value = value;
	return token;
}

Token Token::create_identifier_token(const std::string& value, std::string lexeme, Position pos)
{
	Token token(TokenType::TOKEN_IDENTIFIER, lexeme, pos);
	token.value_.string_value = new std::string(value);
	return token;
}

const std::string Token::toString() const
{
	std::string typeRepr;
	std::string valueRepr;
	std::string posRepr =
		std::to_string(this->pos_.line) + ":"
		+ std::to_string(this->pos_.startChar);

	switch (this->type_)
	{
	case TokenType::TOKEN_INVALID:
		typeRepr = "[INVALID]";
		valueRepr = "ERR";
		break;
	case TokenType::TOKEN_EOF:
		typeRepr = "[EOF]";
		valueRepr = "EOF";
		break;
	case TokenType::TOKEN_KEYWORD:
		typeRepr = "[KEYWORD]";
		valueRepr = this->lexeme_;
		break;
	case TokenType::TOKEN_IDENTIFIER:
		typeRepr = "[IDENTIFIER]";
		valueRepr = this->lexeme_;
		break;
	case TokenType::TOKEN_STRING:
		typeRepr = "[STRING]";
		valueRepr = *this->value_.string_value;
		break;
	case TokenType::TOKEN_OPERATOR:
		typeRepr = "[OPERATOR]";
		valueRepr = this->lexeme_;
		break;
	case TokenType::TOKEN_SYMBOL:
		typeRepr = "[SYMBOL]";
		valueRepr = this->lexeme_;
		break;
	case TokenType::TOKEN_NUMBER:
		typeRepr = "[OPERATOR]";
		valueRepr = this->lexeme_;
	}

	return typeRepr + " at " + posRepr + " is " + valueRepr;
}
