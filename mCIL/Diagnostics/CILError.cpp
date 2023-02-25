#include "CILError.h"

const char* CILError::what() const
{
	return this->msg_.c_str();
}

const Position CILError::range() const
{
	return this->range_;
}

const Position CILError::focus() const
{
	return this->focus_;
}

bool CILError::has_pos() const
{
	return this->has_pos_;
}

void CILError::add_range(Position range)
{
	this->has_pos_ = true;
	this->range_ = range;
	this->focus_ = range;
}

void CILError::add_focus(Position focus)
{
	if (this->has_pos() && this->range_.range().includes(focus.range()))
	{
		this->focus_ = focus;
	}
}

CILError::CILError(const std::string& msg, Position range, Position focus, bool has_pos)
	: msg_(msg), range_(range), focus_(focus), has_pos_(has_pos) {}

void CILError::insert(std::stringstream& ss, int value)
{
	ss << value;
}

void CILError::insert(std::stringstream& ss, double value)
{
	ss << value;
}

void CILError::insert(std::stringstream& ss, char value)
{
	ss << value;
}

void CILError::insert(std::stringstream& ss, size_t value)
{
	ss << value;
}

void CILError::insert(std::stringstream& ss, std::string value)
{
	ss << value;
}

void CILError::insert(std::stringstream& ss, cilType value)
{
	if (value.is_const)
	{
		ss << "const ";
	}
	switch (value.type)
	{
	case Type::BOOL:
		ss << "bool";
		break;
	case Type::NUM:
		ss << "num";
		break;
	case Type::STR:
		ss << "str";
		break;
	case Type::NONE:
		ss << "none";
		break;
	case Type::UNKNOWN:
		ss << "auto";
		break;
	case Type::ERROR:
		ss << "error";
		break;
	default:
		ss << "?";
	}
}
void CILError::insert(std::stringstream& ss, TokenType value)
{
	ss << TokenPrinter::print_token_type(value);
}

void CILError::insert(std::stringstream& ss, Symbol value)
{
	ss << TokenPrinter::print_symbol(value);
}

void CILError::insert(std::stringstream& ss, Operator value)
{
	ss << TokenPrinter::print_operator(value);
}

void CILError::insert(std::stringstream& ss, Keyword value)
{
	ss << TokenPrinter::print_keyword(value);
}

void CILError::insert(std::stringstream& ss, PrimaryType value)
{
	switch (value)
	{
	case PrimaryType::PRIMARY_BOOL:
		ss << "bool";
		break;
	case PrimaryType::PRIMARY_NUM:
		ss << "num";
		break;
	case PrimaryType::PRIMARY_STR:
		ss << "str";
		break;
	case PrimaryType::PRIMARY_NONE:
		ss << "none";
		break;
	case PrimaryType::PRIMARY_IDENTIFIER:
		ss << "identifier";
		break;
	default:
		ss << "?";
	}
}
