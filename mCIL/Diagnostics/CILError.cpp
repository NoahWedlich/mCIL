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

void CILError::insert(std::stringstream& ss, Type value)
{
	//TODO: Move this to a pretty printer class
	if (value.is_const())
	{
		ss << "const ";
	}
	ss << type_name(value.id());
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

void CILError::insert(std::stringstream& ss, ExprType type)
{
	switch (type)
	{
	case ExprType::EXPRESSION_ERROR:
		ss << "error-expr";
		break;
	case ExprType::EXPRESSION_GROUPING:
		ss << "grouping-expr";
		break;
	case ExprType::EXPRESSION_CALL:
		ss << "call-expr";
		break;
	case ExprType::EXPRESSION_ACCESS:
		ss << "access-expr";
		break;
	case ExprType::EXPRESSION_NEW:
		ss << "new-expr";
		break;
	case ExprType::EXPRESSION_ARRAY_ACCESS:
		ss << "array-access-expr";
		break;
	case ExprType::EXPRESSION_PRIMARY:
		ss << "primary-expr";
		break;
	case ExprType::EXPRESSION_UNARY:
		ss << "unary-expr";
		break;
	case ExprType::EXPRESSION_BINARY:
		ss << "binary-expr";
		break;
	case ExprType::EXPRESSION_TERNARY:
		ss << "ternary-expr";
		break;
	case ExprType::EXPRESSION_ASSIGNMENT:
		ss << "assignment-expr";
		break;
	default:
		ss << "unknown-expr";
		break;
	}
}

void CILError::insert(std::stringstream& ss, StmtType type)
{
	switch (type)
	{
	case StmtType::STATEMENT_BLOCK:
		ss << "block-stmt";
		break;
	case StmtType::STATEMENT_BREAK:
		ss << "break-stmt";
		break;
	case StmtType::STATEMENT_RETURN:
		ss << "return-stmt";
		break;
	case StmtType::STATEMENT_PRINT:
		ss << "print-stmt";
		break;
	case StmtType::STATEMENT_ELIF:
		ss << "elif-stmt";
		break;
	case StmtType::STATEMENT_IF:
		ss << "if-stmt";
		break;
	case StmtType::STATEMENT_WHILE:
		ss << "while-stmt";
		break;
	case StmtType::STATEMENT_FOR:
		ss << "for-stmt";
		break;
	case StmtType::STATEMENT_VAR_DECL:
		ss << "var-decl-stmt";
		break;
	case StmtType::STATEMENT_ARR_DECL:
		ss << "array-decl-stmt";
		break;
	case StmtType::STATEMENT_FUNC_DECL:
		ss << "function-decl-stmt";
		break;
	case StmtType::STATEMENT_CLASS_DECL:
		ss << "class-decl-stmt";
		break;
	case StmtType::STATEMENT_EXPR:
		ss << "expr-stmt";
		break;
	default:
		ss << "unknow-stmt";
		break;
	}
}

void CILError::insert(std::stringstream& ss, llvm::Value* value)
{
	std::string v = "";
	llvm::raw_string_ostream os{ v };
	value->print(os);
	ss << v;
}
