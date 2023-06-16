#include "Scanner.h"

Scanner::Scanner(token_list& code)
	: code_(code), table_(), current_(0)
{
}

token_list Scanner::scan()
{
	token_list list;
    while (!at_end())
	{
		if (scan_variable())
			continue;
		if (scan_function())
			continue;
		if (scan_class())
			continue;
		list.push_back(advance());
	}

	table_.make_table_global();
	return list;
}

token_list Scanner::scan_stmt()
{
	if (match_symbol(Symbol::LEFT_BRACE, false))
		return scan_block();

	token_list list{};
	while (!match_symbol(Symbol::SEMICOLON, false))
		list.push_back(advance());
	list.push_back(advance());

	return list;
}

token_list Scanner::scan_block()
{
	token_list list{};
	list.push_back(expect_symbol(Symbol::LEFT_BRACE));

	for (size_t depth = 1; depth > 0;)
	{
		if (match_symbol(Symbol::LEFT_BRACE, false))
			depth++;
		else if (match_symbol(Symbol::RIGHT_BRACE, false))
			depth--;
		list.push_back(advance());
	}

	return list;
}

token_ptr Scanner::peek()
{
	return code_[current_];
}

token_ptr Scanner::advance()
{
	return code_[current_++];
}

token_ptr Scanner::consume()
{
	return code_[++current_];
}

bool Scanner::at_end()
{
	return current_ == code_.size();
}

token_ptr Scanner::expect_number()
{
	if (!match_number(false))
		throw CILError::error(peek()->pos(), "Expected 'number' got '$'", peek()->type());
	return advance();
}

token_ptr Scanner::expect_string()
{
	if (!match_string(false))
		throw CILError::error(peek()->pos(), "Expected 'string' got '$'", peek()->type());
	return advance();
}

token_ptr Scanner::expect_identifier()
{
	if (!match_identifier(false))
		throw CILError::error(peek()->pos(), "Expected 'identifier' got '$'", peek()->type());
	return advance();
}

token_ptr Scanner::expect_symbol(Symbol symbol)
{
	if (peek()->type() != TokenType::TOKEN_SYMBOL)
		throw CILError::error(peek()->pos(), "Expected 'symbol' got '$'", peek()->type());
	else if (!match_symbol(symbol, false))
		throw CILError::error(peek()->pos(), "Expected '$' got '$'", symbol, peek()->symbol());
	return advance();
}

token_ptr Scanner::expect_operator(Operator op)
{
	if (peek()->type() != TokenType::TOKEN_OPERATOR)
		throw CILError::error(peek()->pos(), "Expected 'operator' got '$'", peek()->type());
	else if (!match_operator(op, false))
		throw CILError::error(peek()->pos(), "Expected '$' got '$'", op, peek()->op());
	return advance();
}

token_ptr Scanner::expect_keyword(Keyword keyword)
{
	if (peek()->type() != TokenType::TOKEN_KEYWORD)
		throw CILError::error(peek()->pos(), "Expected 'keyword' got '$'", peek()->type());
	else if (!match_keyword(keyword, false))
		throw CILError::error(peek()->pos(), "Expected '$' got '$'", keyword, peek()->keyword());
	return advance();
}

Type Scanner::expect_type()
{
	TypeQualifier qualifier = 0;
	if (match_keyword(Keyword::KEYWORD_CONST))
	{
		qualifier |= TypeFlags::CONST;
	}

	if (peek()->type() != TokenType::TOKEN_IDENTIFIER)
		throw CILError::error(peek()->pos(), "Expected 'type' got '$'", peek()->type());
	token_ptr type = advance();

	if (!TypeTable::type_exists(type->identifier()))
		//TODO: Add optimistic type creation
		throw CILError::error(peek()->pos(), "Type '$' does not exist", type->identifier());

	return Type::make(type->identifier(), qualifier);
}

bool Scanner::match_number(bool consume)
{
	const token_ptr token = this->peek();
	if (token->is_number())
	{
		if (consume) advance();
		return true;
	}
	return false;
}

bool Scanner::match_string(bool consume)
{
	const token_ptr token = this->peek();
	if (token->is_string())
	{
		if (consume) this->advance();
		return true;
	}
	return false;
}

bool Scanner::match_identifier(bool consume)
{
	const token_ptr token = this->peek();
	if (token->is_identifier())
	{
		if (consume) this->advance();
		return true;
	}
	return false;
}

bool Scanner::match_symbol(Symbol symbol, bool consume)
{
	const token_ptr token = this->peek();
	if (token->is_symbol() && token->symbol() == symbol)
	{
		if (consume) this->advance();
		return true;
	}
	return false;
}

bool Scanner::match_operator(Operator op, bool consume)
{
	const token_ptr token = this->peek();
	if (token->is_operator() && token->op() == op)
	{
		if (consume) this->advance();
		return true;
	}
	return false;
}

bool Scanner::match_keyword(Keyword keyword, bool consume)
{
	const token_ptr token = this->peek();
	if (token->is_keyword() && token->keyword() == keyword)
	{
		if (consume) this->advance();
		return true;
	}
	return false;
}

bool Scanner::match_type(bool consume)
{
	if (match_keyword(Keyword::KEYWORD_CONST, consume))
		return true;
	//TODO: Add optimistic type creation
	if (match_identifier(consume) && TypeTable::type_exists(peek()->identifier()))
		return true;
	return false;
}

bool Scanner::scan_variable()
{
	return false;
}

bool Scanner::scan_function()
{
	if (match_keyword(Keyword::KEYWORD_DEF))
	{
		token_ptr name = expect_identifier();
		expect_symbol(Symbol::LEFT_PAREN);

		std::vector<SymbolTable::Variable> parameters{};
		while (!match_symbol(Symbol::RIGHT_PAREN))
		{
			if (at_end()) throw CILError::error(peek()->pos(), "Expected ')' got 'EOF'");
			Type type = expect_type();
			token_ptr arg_name = expect_identifier();

			if (match_operator(Operator::OPERATOR_EQUAL))
			{
				token_list init{};
				while (!match_symbol(Symbol::COMMA, false) && !match_symbol(Symbol::RIGHT_PAREN, false))
				{ init.push_back(advance()); }
				parameters.push_back(SymbolTable::Variable{ arg_name->identifier(), type, init });
			}

			parameters.push_back(SymbolTable::Variable{ arg_name->identifier(), type });

			if (!match_symbol(Symbol::COMMA))
			{
				expect_symbol(Symbol::RIGHT_PAREN);
				break;
			}
		}

		Type return_type = Type::make("none");
		if (match_symbol(Symbol::ARROW))
			return_type = expect_type();

		if (match_symbol(Symbol::SEMICOLON))
		{
			table_.declare_local_function(name->identifier(), parameters, return_type);
			return true;
		}

		token_list body = scan_stmt();

		table_.declare_local_function(name->identifier(), parameters, return_type, body);
		return true;
	}
	return false;
}

bool Scanner::scan_class()
{
	return false;
}
