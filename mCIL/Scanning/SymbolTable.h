#pragma once
#include "../cil-system.h"
#include "../Types/Type.h"
#include "../Types/TypeTable.h"
#include "../Types/cil-types.h"
#include "../Utils/Threading/ThreadSafeObj.h"
#include "../Diagnostics/CILError.h"
#include "../Lexing/Token.h"

class SymbolTable
{
public:
	SymbolTable();

	void decl_loc_variable(VarInfo info, token_list tokens = {});
	void decl_loc_function(FuncInfo info, token_list tokens = {});
	void decl_loc_class(ClassInfo info, token_list tokens = {});

	static void decl_glob_variable(VarInfo info, token_list tokens = {});
	static void decl_glob_function(FuncInfo info, token_list tokens = {});
	static void decl_glob_class(ClassInfo info, token_list tokens = {});

	const bool exists_loc_variable(std::string name) const;
	const bool exists_loc_function(std::string name) const;
	const bool exists_loc_class(std::string name) const;

	static const bool exists_glob_variable(std::string name);
	static const bool exists_glob_function(std::string name);
	static const bool exists_glob_class(std::string name);

	const VarInfo get_loc_variable(std::string name) const;
	const FuncInfo get_loc_function(std::string name) const;
	const ClassInfo get_loc_class(std::string name) const;

	static const VarInfo get_glob_variable(std::string name);
	static const FuncInfo get_glob_function(std::string name);
	static const ClassInfo get_glob_class(std::string name);

	static void make_table_global(SymbolTable& table);
private:
	struct var_ {
		VarInfo info;
		token_list tokens_;
		expr_ptr expr_;
	};

	struct func_ {
		FuncInfo info;
		token_list tokens_;
		stmt_ptr stmt_;
	};

	struct class_ {
		ClassInfo info;
		token_list tokens_;
		stmt_list stmts_;
	};
private:
	std::unordered_map<std::string, var_> vars_;
	std::unordered_map<std::string, func_> funcs_;
	std::unordered_map<std::string, class_> classes_;

	static ThreadSafeObj<SymbolTable> global_table_;
};