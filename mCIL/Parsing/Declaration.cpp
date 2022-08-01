#include "Declaration.h"

decl_ptr Declaration::make_error_decl(Position pos)
{
	return decl_ptr(new ErrorDeclaration(pos));
}

decl_ptr Declaration::make_var_decl(bool is_const, ObjType type, const std::string& name, expr_ptr val, Position pos)
{
	return decl_ptr(new VarDeclaration(is_const, type, name, val, pos));
}

decl_ptr Declaration::make_stmt_decl(stmt_ptr stmt, Position pos)
{
	return decl_ptr(new StmtDeclaration(stmt, pos));
}
