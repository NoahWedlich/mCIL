#pragma once
#include "../cil-system.h"
#include "../Types/Type.h"
#include "../Types/TypeTable.h"
#include "../Types/cil-types.h"
#include "../Utils/Threading/ThreadSafeObj.h"
#include "../Diagnostics/CILError.h"
#include "../Lexing/Token.h"

class Interpreter;

class SymbolTable
{
public:
	friend Interpreter;

	struct Variable {
		std::string name;
		Type type;

		token_list tokens;
		expr_ptr init_expr = nullptr;
		bool parsed = false;
	};

	struct Function {
		std::string name;
		std::vector<Variable> args;
		Type ret_type;
		//TODO: Add has_return

		token_list tokens;
		stmt_ptr body = nullptr;
		bool parsed = false;
	};

	struct Class {
		std::string name;
		std::vector<Variable> members;
		std::vector<Function> methods;

		token_list tokens;
		bool parsed = false;
	};
public:
	SymbolTable();


	void declare_local_variable(std::string name, Type type, token_list tokens = {});
	void declare_local_function(std::string name, std::vector<Variable> args, Type ret_type, token_list tokens = {});
	void declare_local_class(std::string name, std::vector<Variable> members, std::vector<Function> methods, token_list tokens = {});

	static void declare_global_variable(std::string name, Type type, token_list tokens = {});
	static void declare_global_function(std::string name, std::vector<Variable> args, Type ret_type, token_list tokens = {});
	static void declare_global_class(std::string name, std::vector<Variable> members, std::vector<Function> methods, token_list tokens = {});

	const bool local_variable_exists(std::string name) const;
	const bool local_function_exists(std::string name) const;
	const bool local_class_exists(std::string name) const;

	static const bool global_variable_exists(std::string name);
	static const bool global_function_exists(std::string name);
	static const bool global_class_exists(std::string name);

	Variable& get_local_variable(std::string name);
	Function& get_local_function(std::string name);
	Class& get_local_class(std::string name);

	static Variable& get_global_variable(std::string name);
	static Function& get_global_function(std::string name);
	static Class& get_global_class(std::string name);

	void make_table_global();
private:
	std::unordered_map<std::string, Variable> vars_;
	std::unordered_map<std::string, Function> funcs_;
	std::unordered_map<std::string, Class> classes_;

	static ThreadSafeObj<SymbolTable> global_table_;
};