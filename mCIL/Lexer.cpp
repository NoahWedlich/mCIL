#include "Lexer.h"

std::map<std::string, Keyword> Lexer::keyword_map =
{
	{"NULL"   , Keyword::KEYWORD_NULL   },
	{"num"    , Keyword::KEYWORD_NUM    },
	{"str"    , Keyword::KEYWORD_STR    },
	{"auto"   , Keyword::KEYWORD_AUTO   },
	{"const"  , Keyword::KEYWORD_CONST  },
	{"true"   , Keyword::KEYWORD_TRUE   },
	{"false"  , Keyword::KEYWORD_FALSE  },
	{"if"     , Keyword::KEYWORD_IF     },
	{"else"   , Keyword::KEYWORD_ELSE   },
	{"for"    , Keyword::KEYWORD_FOR    },
	{"while"  , Keyword::KEYWORD_WHILE  },
	{"break"  , Keyword::KEYWORD_BREAK  },
	{"def"    , Keyword::KEYWORD_DEF    },
	{"return" , Keyword::KEYWORD_RETURN },
	{"class"  , Keyword::KEYWORD_CLASS  },
	{"this"   , Keyword::KEYWORD_THIS   },
	{"extents", Keyword::KEYWORD_EXTENDS}
};

Lexer::Lexer(const std::string file_name, std::ifstream& file)
	: file_name_(file_name), file_(file), current_line_(nullptr),
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
	Token currentToken = this->next_token();
	std::vector<Token> tokens{ currentToken };
	while (!currentToken.isEOF())
	{
		currentToken = this->next_token();
		tokens.push_back(currentToken);
	}
	return tokens;
}

Token Lexer::next_token()
{
	while (true)
	{
		if (!this->read_required_line())
		{
			return this->create_eof_token();
		}
		const char* current = this->current_line_ + this->char_off_;
		const char* end = this->current_line_ + this->max_line_size_;

		while (current < end)
		{
			char currentChar = *current;
			switch (currentChar)
			{
			case ' ': case '\t': case '\r':
				current++;
				while (*current == ' ' || *current == '\t' || *current == '\r')
				{
					current++;
				}
				break;
			case '\n':
				current++;
				break;
			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
			{
				bool found = false;
				Token number = this->get_number(found);
				if (found) { return number; }
			}
			default:
				current++;
				this->char_off_ = current - this->current_line_;
				return this->create_invalid_token();
			}
		}
		this->char_off_ = current - this->current_line_;
	}
}

int Lexer::read_line()
{
	size_t current = 0;
	char c;
	while (this->file_.get(c))
	{
		if (current + 1 >= this->max_line_size_)
		{
			//TODO: Error manager
			size_t new_size = 2 * this->max_line_size_ + 1;
			std::cout << "Out of memory\n";
			char* new_buffer = new char[new_size];
			memcpy(new_buffer, this->current_line_, current);
			delete[] this->current_line_;
			this->current_line_ = new_buffer;
			this->max_line_size_ = new_size;
		}
		this->current_line_[current] = c;
		++current;
		
		if (c == '\n')
		{
			break;
		}
	}

	this->current_line_[current] = '\0';

	if (current == 0)
	{ return -1; }
	return current;
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

Position Lexer::position()
{
	return Position{ this->line_off_, this->char_off_ };
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

Token Lexer::get_number(bool& found)
{
	found = false;

	const char* start = this->current_line_ + this->char_off_;
	const char* end = this->current_line_ + this->max_line_size_;

	const char* current = start;

	Position pos = this->position();
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