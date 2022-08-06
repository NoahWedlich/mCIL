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
	DECLARATION_STMT,
	DECLARATION_BLOCK
};

class Declaration
{
public:
	Declaration(DeclType type, Position pos)
		: type_(type), pos_(pos) {}

	virtual ~Declaration() {}

	static decl_ptr make_error_decl(Position pos);
	static decl_ptr make_var_decl(bool is_const, ObjType type, const std::string& name, expr_ptr val, Position pos);
	//TODO: Add functions
	//TODO: Add classes
	static decl_ptr make_stmt_decl(stmt_ptr stmt, Position pos);
	static decl_ptr make_block_decl(decl_list inner, Position pos);

	const Position pos() const
	{ return this->pos_; }

	const DeclType type() const
	{ return this->type_; }

	bool is_error_decl() const
	{ return this->type_ == DeclType::DECLARATION_ERROR; }

	bool is_var_decl() const
	{ return this->type_ == DeclType::DECLARATION_VAR; }

	bool is_stmt_decl() const
	{ return this->type_ == DeclType::DECLARATION_STMT; }

	bool is_block_decl() const
	{ return this->type_ == DeclType::DECLARATION_BLOCK; }

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
	VarDeclaration(bool is_const, ObjType type, const std::string& name, expr_ptr val, Position pos)
		: Declaration(DeclType::DECLARATION_VAR, pos), is_const_(is_const), var_type_(type), name_(name), val_(val) {}

	const bool is_const() const
	{ return this->is_const_; }

	const ObjType var_type() const
	{ return this->var_type_; }

	const std::string& name() const
	{ return this->name_; }

	const expr_ptr val() const
	{ return this->val_; }
private:
	bool is_const_;
	ObjType var_type_;
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


class BlockDeclaration : public Declaration
{
public:
	BlockDeclaration(decl_list inner, Position pos)
		: Declaration(DeclType::DECLARATION_BLOCK, pos), inner_(inner) {}

	const decl_list inner() const
	{ return inner_; }
private:
	decl_list inner_;
};