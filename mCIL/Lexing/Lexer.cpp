#include "Lexer.h"

std::map<std::string, Keyword> Lexer::keyword_map =
{
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
	{"new"    , Keyword::KEYWORD_NEW    },
	{"this"   , Keyword::KEYWORD_THIS   },
	{"extends", Keyword::KEYWORD_EXTENDS}
};

Lexer::Lexer(SourceManager& source)
	: source_(source), current_line_(nullptr),
	max_line_size_(128), cur_line_size_(0), char_off_(0), line_off_(0), start_char_(0)
{
	this->current_line_ = new char[this->max_line_size_];
}

Lexer::~Lexer()
{
	delete[] this->current_line_;
}

token_list Lexer::scan_file()
{
	this->cur_line_size_ = 0;
	this->char_off_ = 0;
	this->line_off_ = 0;
	this->start_char_ = 0;

	token_ptr currentToken = this->create_invalid_token();
	
	token_list tokens{ };
	do
	{
		try
		{
			currentToken = this->next_token();
			tokens.push_back(currentToken);
		}
		catch (CILError& err)
		{
			ErrorManager::cil_error(err);
		}
	} while (!currentToken->is_EOF());
	return tokens;
}

token_ptr Lexer::next_token()
{
	while (true)
	{
		if (!this->read_required_line())
		{
			return this->create_eof_token();
		}

		if(this->skip_comments())
		{ continue; }

		if (*(this->current_line_ + this->char_off_) == '\n')
		{
			this->char_off_++;
			continue;
		}

		if(skip_spaces())
		{ continue; }

		token_ptr next = this->create_invalid_token();
		bool found = false;

		this->start_char_ = this->char_off_;
		
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

		throw CILError::error(this->pos(1), "Invalid character '$'", this->current_line_ + this->char_off_);
		this->char_off_++;
		return next;
	}
}

bool Lexer::char_is_alpha(char c)
{
	return ((c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') ||
			(c == '_'));
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

Position Lexer::pos(size_t len)
{
	SourcePos start{ this->line_off_ - 1, this->char_off_ };
	SourcePos end{ this->line_off_ - 1, this->char_off_ + len - 1 };
	return Position{ start, end };
}

token_ptr Lexer::create_invalid_token()
{
	return Token::create_invalid_token(this->pos(1));
}

token_ptr Lexer::create_eof_token()
{
	return Token::create_eof_token(this->pos(1));
}

token_ptr Lexer::create_keyword_token(Keyword type, std::string lexeme, size_t len)
{
	return Token::create_keyword_token(type, lexeme, this->pos(len));
}

token_ptr Lexer::create_string_token(const std::string& text, std::string lexeme, size_t len)
{
	return Token::create_string_token(text, lexeme, this->pos(len));
}

token_ptr Lexer::create_number_token(double value, std::string lexeme, size_t len)
{
	return Token::create_number_token(value, lexeme, this->pos(len));
}

token_ptr Lexer::create_identifier_token(const std::string& value, std::string lexeme, size_t len)
{
	return Token::create_identifier_token(value, lexeme, this->pos(len));
}

token_ptr Lexer::create_operator_token(Operator type, std::string lexeme, size_t len)
{
	return Token::create_operator_token(type, lexeme, this->pos(len));
}

token_ptr Lexer::create_symbol_token(Symbol type, std::string lexeme, size_t len)
{
	return Token::create_symbol_token(type, lexeme, this->pos(len));
}

bool Lexer::skip_spaces()
{
	const char* start = this->current_line_ + this->char_off_;

	const char* current = start;

	if (*current == ' ' || *current == '\t' || *current == '\r')
	{
		current++;
		while (*current == ' ' || *current == '\t' || *current == '\r')
		{
			current++;
		}
		this->char_off_ = current - this->current_line_;
		return true;
	}
	return false;
}

bool Lexer::skip_comments()
{
	const char* current = this->current_line_ + this->char_off_;
	if (*current == '/')
	{
		current++;
		if (*current == '/')
		{
			while (*current != '\n')
			{
				current++;
			}
		}
		else if (*current == '*')
		{
			while (true)
			{
				current++;
				if (*current == '\n')
				{
					this->char_off_ = (++current) - this->current_line_;
					if (!this->read_required_line())
					{ throw CILError::error(this->pos(1), "Expected '*/'"); }
					current = this->current_line_ + this->char_off_;
				}
				if (*current == '*' && *(++current) == '/')
				{
					current++;
					break;
				}
			}
		}
		else
		{ return false; }
	}
	else
	{ return false; }
	this->char_off_ = current - this->current_line_;
	return true;
}

token_ptr Lexer::get_symbol(bool& found)
{
	found = true;
	const char* current = this->current_line_ + this->char_off_;

	token_ptr sym = this->create_invalid_token();

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
	case '-':
		current++;
		if (*current == '>')
		{
			sym = this->create_symbol_token(Symbol::ARROW, "->", 2);
			this->char_off_++;
			break;
		}
		current--;
		found = false;
		return sym;
	default:
		found = false;
		return sym;
	}
	this->char_off_++;
	return sym;
}

token_ptr Lexer::get_operator(bool& found)
{
	found = true;
	const char* current = this->current_line_ + this->char_off_;

	token_ptr op = this->create_invalid_token();

	switch (*current)
	{
	case '+':
		current++;
		if (*current == '+')
		{
			op = this->create_operator_token(Operator::OPERATOR_INCREMENT, "++", 2);
			current++;
			break;
		}
		op = this->create_operator_token(Operator::OPERATOR_ADD, "+");
		break;
	case '-':
		current++;
		if (*current == '-')
		{
			op = this->create_operator_token(Operator::OPERATOR_DECREMENT, "--", 2);
			current++;
			break;
		}
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
			op = this->create_operator_token(Operator::OPERATOR_EQUAL_EQUAL, "==", 2);
			current++;
			break;
		}
		op = this->create_operator_token(Operator::OPERATOR_EQUAL, "=");
		break;
	case '!':
		current++;
		if (*current == '=')
		{
			op = this->create_operator_token(Operator::OPERATOR_NOT_EQUAL, "!=", 2);
			current++;
			break;
		}
		op = this->create_operator_token(Operator::OPERATOR_BANG, "!");
		break;
	case '<':
		current++;
		if (*current == '=')
		{
			op = this->create_operator_token(Operator::OPERATOR_LESS_EQUAL, "<=", 2);
			current++;
			break;
		}
		if (*current == '<')
		{
			op = this->create_operator_token(Operator::OPERATOR_LEFT_BITSHIFT, "<<", 2);
			current++;
			break;
		}
		op = this->create_operator_token(Operator::OPERATOR_LESS, "<");
		break;
	case '>':
		current++;
		if (*current == '=')
		{
			op = this->create_operator_token(Operator::OPERATOR_GREATER_EQUAL, ">=", 2);
			current++;
			break;
		}
		if (*current == '>')
		{
			op = this->create_operator_token(Operator::OPERATOR_RIGHT_BITSHIFT, ">>", 2);
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
			op = this->create_operator_token(Operator::OPERATOR_AND, "&&", 2);
			current++;
			break;
		}
		op = create_operator_token(Operator::OPERATOR_BITWISE_AND, "&");
		break;
	}
	case '|':
		current++;
		if (*current == '|')
		{
			op = this->create_operator_token(Operator::OPERATOR_OR, "||", 2);
			current++;
			break;
		}
		op = create_operator_token(Operator::OPERATOR_BITWISE_OR, "|");
		break;
	case '~':
		current++;
		op = create_operator_token(Operator::OPERATOR_BITWISE_NOT, "~");
		break;
	case '^':
		current++;
		op = create_operator_token(Operator::OPERATOR_BITWISE_XOR, "^");
		break;
	default:
		found = false;
		return op;
	}

	this->char_off_ = current - this->current_line_;
	return op;
}

token_ptr Lexer::get_number(bool& found)
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
		token_ptr token = this->create_number_token(number, number_str, number_str.length());
		this->char_off_ = current - this->current_line_;
		return token;
	}
	return this->create_invalid_token();
}

token_ptr Lexer::get_string(bool& found)
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
			throw CILError::error(this->pos(1), "Unterminated string");
			return this->create_invalid_token();
		}
		str += *current;
		current++;
	}

	found = true;
	token_ptr token = this->create_string_token(str, "\"" + str + "\"", str.length() + 2);
	this->char_off_ = current + 1 - this->current_line_;
	return token;
}

token_ptr Lexer::get_keyword(bool& found)
{
	found = false;

	const char* current = this->current_line_ + this->char_off_;
	const char* end = this->current_line_ + this->cur_line_size_;

	std::string key_str = "";

	if (!this->char_is_alpha(*current))
	{
		return this->create_invalid_token();
	}
	while (this->char_is_alpha(*current))
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
	token_ptr token = this->create_keyword_token(code_it->second, key_str, key_str.length());
	this->char_off_ = current - this->current_line_;
	return token;
}

token_ptr Lexer::get_identifier(bool& found)
{
	found = false;

	const char* current = this->current_line_ + this->char_off_;
	const char* end = this->current_line_ + this->cur_line_size_;

	std::string id_str = "";

	if (!this->char_is_alpha(*current))
	{
		return this->create_invalid_token();
	}
	while (this->char_is_alpha(*current))
	{
		id_str += *current;
		current++;
	}
	
	found = true;
	token_ptr token = this->create_identifier_token(id_str, id_str, id_str.length());
	this->char_off_ = current - this->current_line_;
	return token;
}