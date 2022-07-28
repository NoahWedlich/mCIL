#include "Lexer.h"

std::map<std::string, Keyword> Lexer::keyword_map =
{
	{"none"   , Keyword::KEYWORD_NONE   },
	{"bool"   , Keyword::KEYWORD_BOOL   },
	{"num"    , Keyword::KEYWORD_NUM    },
	{"str"    , Keyword::KEYWORD_STR    },
	{"auto"   , Keyword::KEYWORD_AUTO   },
	{"const"  , Keyword::KEYWORD_CONST  },
	{"true"   , Keyword::KEYWORD_TRUE   },
	{"false"  , Keyword::KEYWORD_FALSE  },
	{"print"  , Keyword::KEYWORD_PRINT  },
	{"if"     , Keyword::KEYWORD_IF     },
	{"elif"   , Keyword::KEYWORD_ELIF   },
	{"else"   , Keyword::KEYWORD_ELSE   },
	{"for"    , Keyword::KEYWORD_FOR    },
	{"while"  , Keyword::KEYWORD_WHILE  },
	{"break"  , Keyword::KEYWORD_BREAK  },
	{"def"    , Keyword::KEYWORD_DEF    },
	{"return" , Keyword::KEYWORD_RETURN },
	{"class"  , Keyword::KEYWORD_CLASS  },
	{"this"   , Keyword::KEYWORD_THIS   },
	{"extends", Keyword::KEYWORD_EXTENDS}
};

Lexer::Lexer(SourceManager& source)
	: source_(source), current_line_(nullptr),
	  max_line_size_(128), cur_line_size_(0), char_off_(0), line_off_(0)
{
	this->current_line_ = new char[this->max_line_size_];
}

Lexer::~Lexer()
{
	delete[] this->current_line_;
}

std::vector<Token> Lexer::scan_file()
{
	this->cur_line_size_ = 0;
	this->char_off_ = 0;
	this->line_off_ = 0;

	try
	{
		Token currentToken = this->next_token();
	
		std::vector<Token> tokens{ currentToken };
		while (!currentToken.is_EOF())
		{
			try
			{
				currentToken = this->next_token();
				tokens.push_back(currentToken);
			}
			catch (LexerError& err)
			{
				ErrorManager::cil_lexer_error(err);
			}
		}
		return tokens;
		}
	catch (LexerError& err)
	{
		ErrorManager::cil_lexer_error(err);
		return std::vector<Token> { Token::create_eof_token(Position{0, 0}) };
	}
}

Token Lexer::next_token()
{
	while (true)
	{
		if (!this->read_required_line())
		{
			return this->create_eof_token();
		}

		if (*(this->current_line_ + this->char_off_) == '\n')
		{
			this->char_off_++;
			continue;
		}

		this->skip_spaces();

		Token next = this->create_invalid_token();
		bool found = false;
		
		//TODO: Add comments
		next = this->get_symbol(found);
		if (found) { return next; }
		next = this->get_operator(found);
		if (found) { return next; }
		next = this->get_number(found);
		if (found) { return next; }
		next = this->get_string(found);
		if (found) { return next; }
		next = this->get_keyword(found);
		if (found) { return next; }
		next = this->get_identifier(found);
		if (found) { return next; }

		throw LexerError("Invalid character", this->position());
		this->char_off_++;
		return next;
	}
}

int Lexer::read_line()
{
	if (this->source_.is_at_end())
	{ return -1; }
	
	size_t length = 0;
	while (!this->source_.get_next_line(this->current_line_, this->max_line_size_, length))
	{
		size_t new_size = 2 * this->max_line_size_ + 1;
		ErrorManager::cil_warning(Position{ this->line_off_ - 1, 0 }, "Out of memory for reading line");
		char* new_buffer = new char[new_size];
		delete[] this->current_line_;
		this->current_line_ = new_buffer;
		this->max_line_size_ = new_size;
	}
	return (int)length;
}

bool Lexer::read_required_line()
{
	if (this->char_off_ < this->cur_line_size_)
	{ return true; }

	int size = this->read_line();
	if (size < 0)
	{ return false; }

	this->line_off_++;
	this->cur_line_size_ = size;
	this->char_off_ = 0;
	return true;
}

Position Lexer::position(int offset)
{
	return Position{ this->line_off_ - 1, this->char_off_ + offset};
}

Token Lexer::create_invalid_token()
{
	return Token::create_invalid_token(this->position());
}

Token Lexer::create_eof_token()
{
	return Token::create_eof_token(this->position());
}

Token Lexer::create_keyword_token(Keyword type, std::string lexeme)
{
	return Token::create_keyword_token(type, lexeme, this->position());
}

Token Lexer::create_string_token(const std::string& text, std::string lexeme)
{
	return Token::create_string_token(text, lexeme, this->position());
}

Token Lexer::create_number_token(double value, std::string lexeme)
{
	return Token::create_number_token(value, lexeme, this->position());
}

Token Lexer::create_identifier_token(const std::string& value, std::string lexeme)
{
	return Token::create_identifier_token(value, lexeme, this->position());
}

Token Lexer::create_operator_token(Operator type, std::string lexeme)
{
	return Token::create_operator_token(type, lexeme, this->position());
}

Token Lexer::create_symbol_token(Symbol type, std::string lexeme)
{
	return Token::create_symbol_token(type, lexeme, this->position());
}

void Lexer::skip_spaces()
{
	const char* start = this->current_line_ + this->char_off_;
	const char* end = this->current_line_ + this->cur_line_size_;

	const char* current = start;

	if (*current == ' ' || *current == '\t' || *current == '\r')
	{
		current++;
		while (*current == ' ' || *current == '\t' || *current == '\r')
		{
			current++;
		}
		this->char_off_ = current - this->current_line_;
		return;
	}
}

Token Lexer::get_symbol(bool& found)
{
	found = true;
	const char* current = this->current_line_ + this->char_off_;

	Token sym = this->create_invalid_token();

	switch (*current)
	{
	case '(':
		sym = this->create_symbol_token(Symbol::LEFT_PAREN, "(");
		break;
	case ')':
		sym = this->create_symbol_token(Symbol::RIGHT_PAREN, ")");
		break;
	case '{':
		sym = this->create_symbol_token(Symbol::LEFT_BRACE, "{");
		break;
	case '}':
		sym = this->create_symbol_token(Symbol::RIGHT_BRACE, "}");
		break;
	case '[':
		sym = this->create_symbol_token(Symbol::LEFT_BRACKET, "[");
		break;
	case ']':
		sym = this->create_symbol_token(Symbol::RIGHT_BRACKET, "]");
		break;
	case ';':
		sym = this->create_symbol_token(Symbol::SEMICOLON, ";");
		break;
	case ',':
		sym = this->create_symbol_token(Symbol::COMMA, ",");
		break;
	case ':':
		sym = this->create_symbol_token(Symbol::COLON, ":");
		break;
	case '?':
		sym = this->create_symbol_token(Symbol::QUERY, "?");
		break;
	case '.':
		sym = this->create_symbol_token(Symbol::DOT, ".");
		break;
	default:
		found = false;
		return sym;
	}
	this->char_off_++;
	return sym;
}

Token Lexer::get_operator(bool& found)
{
	found = true;
	const char* current = this->current_line_ + this->char_off_;

	Token op = this->create_invalid_token();

	switch (*current)
	{
	case '+':
		current++;
		op = this->create_operator_token(Operator::OPERATOR_ADD, "+");
		break;
	case '-':
		current++;
		op = this->create_operator_token(Operator::OPERATOR_SUBTRACT, "-");
		break;
	case '*':
		current++;
		op = this->create_operator_token(Operator::OPERATOR_MULTIPLY, "*");
		break;
	case '/':
		op = this->create_operator_token(Operator::OPERATOR_DIVIDE, "/");
		current++;
		break;
	case '=':
		current++;
		if (*current == '=')
		{
			op = this->create_operator_token(Operator::OPERATOR_EQUAL_EQUAL, "==");
			current++;
			break;
		}
		op = this->create_operator_token(Operator::OPERATOR_EQUAL, "=");
		break;
	case '!':
		current++;
		if (*current == '=')
		{
			op = this->create_operator_token(Operator::OPERATOR_NOT_EQUAL, "!=");
			current++;
			break;
		}
		op = this->create_operator_token(Operator::OPERATOR_BANG, "!");
		break;
	case '<':
		current++;
		if (*current == '=')
		{
			op = this->create_operator_token(Operator::OPERATOR_LESS_EQUAL, "<=");
			current++;
			break;
		}
		op = this->create_operator_token(Operator::OPERATOR_LESS, "<");
		break;
	case '>':
		current++;
		if (*current == '=')
		{
			op = this->create_operator_token(Operator::OPERATOR_GREATER_EQUAL, ">=");
			current++;
			break;
		}
		op = this->create_operator_token(Operator::OPERATOR_GREATER, ">");
		break;
	case '&':
	{
		current++;
		if (*current == '&')
		{
			op = this->create_operator_token(Operator::OPERATOR_AND, "&&");
			current++;
			break;
		}
		this->char_off_ = current - this->current_line_;
		throw LexerError("Invalid operator, did you mean '&&'", this->position(-1));
		return op;
	}
	case '|':
		current++;
		if (*current == '|')
		{
			op = this->create_operator_token(Operator::OPERATOR_OR, "||");
			current++;
			break;
		}
		this->char_off_ = current - this->current_line_;
		throw LexerError("Invalid operator, did you mean '||'", this->position(-1));
		return op;
	default:
		found = false;
		return op;
	}

	this->char_off_ = current - this->current_line_;
	return op;
}

Token Lexer::get_number(bool& found)
{
	const char* start = this->current_line_ + this->char_off_;
	const char* end = this->current_line_ + this->cur_line_size_;

	const char* current = start;

	std::string number_str = "";

	while (current < end)
	{
		if (*current >= '0' && *current <= '9')
		{
			number_str.push_back(*current);
			current++;
		}
		else
		{
			break;
		}
	}
	if (current < end && *current == '.')
	{
		number_str.push_back(*current);
		current++;
		while (current < end)
		{
			if (*current >= '0' && *current <= '9')
			{
				number_str.push_back(*current);
				current++;
			}
			else
			{
				break;
			}
		}
	}
	if (number_str != "")
	{
		found = true;
		double number = std::stod(number_str);
		Token token = this->create_number_token(number, number_str);
		this->char_off_ = current - this->current_line_;
		return token;
	}
	return this->create_invalid_token();
}

Token Lexer::get_string(bool& found)
{
	found = false;

	const char* current = this->current_line_ + this->char_off_;
	const char* end = this->current_line_ + this->cur_line_size_;

	std::string str = "";

	if (*current != '\"')
	{
		return this->create_invalid_token();
	}
	current++;
	while (*current != '\"')
	{
		if (current == end)
		{
			this->char_off_ = current - this->current_line_;
			found = true;
			throw LexerError("Unterminated string", this->position());
			return this->create_invalid_token();
		}
		str += *current;
		current++;
	}

	found = true;
	Token token = this->create_string_token(str, "\"" + str + "\"");
	this->char_off_ = current + 1 - this->current_line_;
	return token;
}

Token Lexer::get_keyword(bool& found)
{
	found = false;

	const char* current = this->current_line_ + this->char_off_;
	const char* end = this->current_line_ + this->cur_line_size_;

	std::string key_str = "";

	if (*current < 'a' || *current > 'z')
	{
		return this->create_invalid_token();
	}
	while ( current < end &&
		(*current != ' '  &&
		 *current != '\t' &&
		 *current != '\r' &&
		 *current != '\n'))
	{
		key_str += *current;
		current++;
	}
	
	auto code_it = this->keyword_map.find(key_str);

	if (code_it == this->keyword_map.end())
	{
		return this->create_invalid_token();
	}

	found = true;
	Token token = this->create_keyword_token(code_it->second, key_str);
	this->char_off_ = current - this->current_line_;
	return token;
}

Token Lexer::get_identifier(bool& found)
{
	found = false;

	const char* current = this->current_line_ + this->char_off_;
	const char* end = this->current_line_ + this->cur_line_size_;

	std::string id_str = "";

	if ((*current < 'A' || *current > 'Z') &&
		(*current < 'a' || *current > 'z') &&
		(*current != '_'))
	{
		return this->create_invalid_token();
	}
	while (current < end &&
		(*current != ' ' &&
		*current != '\t' &&
		*current != '\r' &&
		*current != '\n'))
	{
		id_str += *current;
		current++;
	}
	
	found = true;
	Token token = this->create_identifier_token(id_str, id_str);
	this->char_off_ = current - this->current_line_;
	return token;
}