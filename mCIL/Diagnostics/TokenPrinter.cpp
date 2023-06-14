#include "TokenPrinter.h"

namespace TokenPrinter
{
	std::string print_symbol(Symbol s)
	{
		switch (s)
		{
		case Symbol::LEFT_PAREN:
			return "(";
		case Symbol::RIGHT_PAREN:
			return ")";
		case Symbol::LEFT_BRACE:
			return "{";
		case Symbol::RIGHT_BRACE:
			return "}";
		case Symbol::LEFT_BRACKET:
			return "[";
		case Symbol::RIGHT_BRACKET:
			return "]";
		case Symbol::SEMICOLON:
			return ";";
		case Symbol::COMMA:
			return "";
		case Symbol::DOT:
			return ".";
		case Symbol::COLON:
			return ":";
		case Symbol::QUERY:
			return "?";
		default:
			return "INVALID_SYMBOL";
		}
	}

	std::string print_operator(Operator op)
	{
		switch (op)
		{
		case Operator::OPERATOR_BANG:
			return "!";
		case Operator::OPERATOR_ADD:
			return "+";
		case Operator::OPERATOR_INCREMENT:
			return "++";
		case Operator::OPERATOR_DECREMENT:
			return "--";
		case Operator::OPERATOR_SUBTRACT:
			return "-";
		case Operator::OPERATOR_MULTIPLY:
			return "*";
		case Operator::OPERATOR_DIVIDE:
			return "/";
		case Operator::OPERATOR_EQUAL:
			return "=";
		case Operator::OPERATOR_EQUAL_EQUAL:
			return "==";
		case Operator::OPERATOR_NOT_EQUAL:
			return "!=";
		case Operator::OPERATOR_LESS:
			return "<";
		case Operator::OPERATOR_GREATER:
			return ">";
		case Operator::OPERATOR_LESS_EQUAL:
			return "<=";
		case Operator::OPERATOR_GREATER_EQUAL:
			return ">=";
		case Operator::OPERATOR_AND:
			return "&&";
		case Operator::OPERATOR_OR:
			return "||";
		case Operator::OPERATOR_LEFT_BITSHIFT:
			return "<<";
		case Operator::OPERATOR_RIGHT_BITSHIFT:
			return ">>";
		case Operator::OPERATOR_BITWISE_NOT:
			return "~";
		case Operator::OPERATOR_BITWISE_AND:
			return "&";
		case Operator::OPERATOR_BITWISE_OR:
			return "|";
		case Operator::OPERATOR_BITWISE_XOR:
			return "^";
		default:
			return "INVALID_OPERATOR";
		}
	}

	std::string print_keyword(Keyword k)
	{
		switch (k)
		{
		case Keyword::KEYWORD_NONE:
			return "none";
		case Keyword::KEYWORD_BOOL:
			return "bool";
		case Keyword::KEYWORD_NUM:
			return "num";
		case Keyword::KEYWORD_STR:
			return "str";
		case Keyword::KEYWORD_AUTO:
			return "auto";
		case Keyword::KEYWORD_CONST:
			return "const";
		case Keyword::KEYWORD_TRUE:
			return "true";
		case Keyword::KEYWORD_FALSE:
			return "false";
		case Keyword::KEYWORD_IF:
			return "if";
		case Keyword::KEYWORD_ELIF:
			return "elif";
		case Keyword::KEYWORD_ELSE:
			return "else";
		case Keyword::KEYWORD_FOR:
			return "for";
		case Keyword::KEYWORD_WHILE:
			return "while";
		case Keyword::KEYWORD_BREAK:
			return "break";
		case Keyword::KEYWORD_DEF:
			return "def";
		case Keyword::KEYWORD_RETURN:
			return "return";
		case Keyword::KEYWORD_CLASS:
			return "class";
		case Keyword::KEYWORD_THIS:
			return "this";
		case Keyword::KEYWORD_EXTENDS:
			return "extends";
		default:
			return "INVALID_KEYWORD";
		}
	}

	std::string print_token(token_ptr t)
	{
		switch (t->type())
		{
		case TokenType::TOKEN_INVALID:
			return "INVALID_TOKEN";
		case TokenType::TOKEN_EOF:
			return "EOF";
		case TokenType::TOKEN_SYMBOL:
			return print_symbol(t->symbol());
		case TokenType::TOKEN_OPERATOR:
			return print_operator(t->op());
		case TokenType::TOKEN_KEYWORD:
			return print_keyword(t->keyword());
		case TokenType::TOKEN_STRING:
			return t->string_val();
		case TokenType::TOKEN_NUMBER:
			return std::to_string(t->number_val());
		case TokenType::TOKEN_IDENTIFIER:
			return t->identifier();
		default:
			return "INVALID_TOKEN";
		}
	}

	std::string print_token_type(TokenType type)
	{
		switch (type)
		{
		case TokenType::TOKEN_INVALID:
			return "invalid";
		case TokenType::TOKEN_EOF:
			return "eof";
		case TokenType::TOKEN_SYMBOL:
			return "symbol";
		case TokenType::TOKEN_OPERATOR:
			return "operator";
		case TokenType::TOKEN_KEYWORD:
			return "keyword";
		case TokenType::TOKEN_STRING:
			return "string";
		case TokenType::TOKEN_NUMBER:
			return "number";
		case TokenType::TOKEN_IDENTIFIER:
			return "identifier";
		default:
			return "?";
		}
	}

}