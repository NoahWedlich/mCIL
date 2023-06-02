#pragma once
#include "../cil-system.h"
#include "../Types/Type.h"
#include "../Types/TypeTable.h"
#include "../Types/cil-types.h"
#include "../Utils/Threading/ThreadSafeObj.h"

class SymbolTable
{
public:
	//TODO: Add definitions and the respective code-blocks
	SymbolTable();

	void decl_loc_variable(VarInfo info);
	void decl_loc_function(FuncInfo info);
	void decl_loc_class(ClassInfo info);

	static void decl_glob_variable(VarInfo info);
	static void decl_glob_function(FuncInfo info);
	static void decl_glob_class(ClassInfo info);

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
	std::unordered_map<std::string, VarInfo> vars_;
	std::unordered_map<std::string, FuncInfo> funcs_;
	std::unordered_map<std::string, ClassInfo> classes_;

	static ThreadSafeObj<SymbolTable> global_table_;
};