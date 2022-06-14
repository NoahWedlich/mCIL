#include "Lexer.h"

Lexer::Lexer(const char* filepath)
	: file(filepath), line(0), curChar(0) {}

Lexer::~Lexer() { file.close(); }

std::vector<Token> Lexer::scan()
{
	char current;
	std::string lexeme = "";
	while (file.get(current))
	{
		lexeme += current;
		switch (current)
		{
		case '\n':
			line++;
			curChar = -1;
			lexeme = "";
			break;
		case '(':
			tokens.push_back(Token::create_symbol_token(Symbol::LEFT_PAREN, lexeme, Position{ line, curChar }));
			lexeme = "";
			break;
		case ')':
			tokens.push_back(Token::create_symbol_token(Symbol::RIGHT_PAREN, lexeme, Position{ line, curChar }));
			lexeme = "";
			break;
		case '{':
			tokens.push_back(Token::create_symbol_token(Symbol::LEFT_BRACE, lexeme, Position{ line, curChar }));
			lexeme = "";
			break;
		case '}':
			tokens.push_back(Token::create_symbol_token(Symbol::RIGHT_BRACE, lexeme, Position{ line, curChar }));
			lexeme = "";
			break;
		case '[':
			tokens.push_back(Token::create_symbol_token(Symbol::LEFT_BRACKET, lexeme, Position{ line, curChar }));
			lexeme = "";
			break;
		case ']':
			tokens.push_back(Token::create_symbol_token(Symbol::RIGHT_BRACKET, lexeme, Position{ line, curChar }));
			lexeme = "";
			break;
		default:
			tokens.push_back(Token::create_invalid_token(Position{ line, curChar }));
			lexeme = "";
		}
		curChar++;
	}
	tokens.push_back(Token::create_eof_token(Position{ line, curChar }));
	return tokens;
}

void Lexer::dumpTokens()
{
	for (Token tok : tokens)
	{
		std::cout << tok.toString() << std::endl;
	}
}

std::map<std::string, Symbol> Lexer::symbolMap =
{
	{"(", Symbol::LEFT_PAREN   },
	{")", Symbol::RIGHT_PAREN  },
	{"{", Symbol::LEFT_BRACE   },
	{"}", Symbol::RIGHT_BRACE  },
	{"[", Symbol::LEFT_BRACKET },
	{"]", Symbol::RIGHT_BRACKET},
	{";", Symbol::SEMICOLON    },
	{",", Symbol::COMMA        },
	{".", Symbol::DOT          },
	{":", Symbol::COLON        }
};

std::map<std::string, Operator> Lexer::operatorMap =
{
	{"!" , Operator::OPERATOR_BANG         },
	{"+" , Operator::OPERATOR_ADD          },
	{"-" , Operator::OPERATOR_SUBTRACT     },
	{"*" , Operator::OPERATOR_MULTIPLY     },
	{"/" , Operator::OPERATOR_DIVIDE       },
	{"=" , Operator::OPERATOR_EQUAL        },
	{"==", Operator::OPERATOR_EQUAL_EQUAL  },
	{"<" , Operator::OPERATOR_LESS         },
	{"<=", Operator::OPERATOR_LESS_EQUAL   },
	{">" , Operator::OPERATOR_GREATER      },
	{">=", Operator::OPERATOR_GREATER_EQUAL},
	{"&&", Operator::OPERATOR_AND          },
	{"||", Operator::OPERATOR_OR           }
};

std::map<std::string, Keyword> Lexer::keywordMap =
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

