#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Position.h"
#include "../Lexing/Token.h"
#include "Expression.h"
#include "Statement.h"
#include "../Interpreting/Object.h"

class Declaration;
typedef std::shared_ptr<Declaration> decl_ptr;
typedef std::vector<decl_ptr> decl_list;

enum class DeclType
{
	DECLARATION_ERROR,
	DECLARATION_VAR,
	DECLARATION_FUN,
	DECLARATION_CLASS,
	DECLARATION_STMT
};

class Declaration
{
public:
	Declaration(DeclType type, Position pos)
		: type_(type), pos_(pos) {}

	virtual ~Declaration() {}

	static decl_ptr make_error_decl(Position pos);
	static decl_ptr make_var_decl(ObjType type, const std::string& name, expr_ptr val, Position pos);
	//TODO: Add functions
	//TODO: Add classes
	static decl_ptr make_stmt_decl(stmt_ptr stmt, Position pos);

	const Position pos() const
	{
		return this->pos_;
	}

	const DeclType type() const
	{
		return this->type_;
	}

	bool is_error_decl() const
	{
		return this->type_ == DeclType::DECLARATION_ERROR;
	}

	bool is_var_decl() const
	{
		return this->type_ == DeclType::DECLARATION_VAR;
	}

	bool is_stmt_decl() const
	{
		return this->type_ == DeclType::DECLARATION_STMT;
	}

private:
	DeclType type_;
	Position pos_;
};

class ErrorDeclaration : public Declaration
{
public:
	ErrorDeclaration(Position pos)
		: Declaration(DeclType::DECLARATION_ERROR, pos) {}
};

class VarDeclaration : public Declaration
{
public:
	VarDeclaration(ObjType type, const std::string& name, expr_ptr val, Position pos)
		: Declaration(DeclType::DECLARATION_VAR, pos), type_(type), name_(name), val_(val) {}

	const ObjType type() const
	{ return this->type_; }

	const std::string& name() const
	{ return this->name_; }

	const expr_ptr val() const
	{ return this->val_; }
private:
	ObjType type_;
	const std::string& name_;
	expr_ptr val_;
};

class StmtDeclaration : public Declaration
{
public:
	StmtDeclaration(stmt_ptr inner, Position pos)
		: Declaration(DeclType::DECLARATION_STMT, pos), inner_(inner) {}

	const stmt_ptr inner() const
	{ return this->inner_; }
private:
	stmt_ptr inner_;
};