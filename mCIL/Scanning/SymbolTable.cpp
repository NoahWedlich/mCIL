#include "SymbolTable.h"

ThreadSafeObj<SymbolTable> SymbolTable::global_table_{};

SymbolTable::SymbolTable()
    : vars_(), funcs_(), classes_()
{
}

void SymbolTable::declare_local_variable(std::string name, Type type, token_list tokens)
{
    if (!vars_.contains(name))
        vars_.emplace(name, Variable(name, type, tokens));
    else if (vars_.at(name).tokens.empty())
		vars_.at(name).tokens = tokens;
	else
		throw CILError::error("Variable $ already defined", name);
}

void SymbolTable::declare_local_function(std::string name, std::vector<Variable> args, Type ret_type, token_list tokens)
{
	if (!funcs_.contains(name))
		funcs_.emplace(name, Function(name, args, ret_type, tokens));
	else if (funcs_.at(name).tokens.empty())
		funcs_.at(name).tokens = tokens;
	else
		throw CILError::error("Function $ already defined", name);
}

void SymbolTable::declare_local_class(std::string name, std::vector<Variable> members, std::vector<Function> methods, token_list tokens)
{
	if (!classes_.contains(name))
		classes_.emplace(name, Class(name, members, methods, tokens));
	else if (classes_.at(name).tokens.empty())
		classes_.at(name).tokens = tokens;
	else
		throw CILError::error("Class $ already defined", name);
}

void SymbolTable::declare_global_variable(std::string name, Type type, token_list tokens)
{ global_table_->declare_local_variable(name, type, tokens); }

void SymbolTable::declare_global_function(std::string name, std::vector<Variable> args, Type ret_type, token_list tokens)
{ global_table_->declare_local_function(name, args, ret_type, tokens); }

void SymbolTable::declare_global_class(std::string name, std::vector<Variable> members, std::vector<Function> methods, token_list tokens)
{ global_table_->declare_local_class(name, members, methods, tokens); }

const bool SymbolTable::local_variable_exists(std::string name) const
{ return vars_.contains(name); }

const bool SymbolTable::local_function_exists(std::string name) const
{ return funcs_.contains(name); }

const bool SymbolTable::local_class_exists(std::string name) const
{ return classes_.contains(name); }

const bool SymbolTable::global_variable_exists(std::string name)
{ return SymbolTable::global_table_->local_variable_exists(name); }

const bool SymbolTable::global_function_exists(std::string name)
{ return SymbolTable::global_table_->local_function_exists(name); }

const bool SymbolTable::global_class_exists(std::string name)
{ return SymbolTable::global_table_->local_class_exists(name); }

SymbolTable::Variable& SymbolTable::get_local_variable(std::string name)
{ return vars_.at(name); }

SymbolTable::Function& SymbolTable::get_local_function(std::string name)
{ return funcs_.at(name); }

SymbolTable::Class& SymbolTable::get_local_class(std::string name)
{ return classes_.at(name); }

SymbolTable::Variable& SymbolTable::get_global_variable(std::string name)
{ return SymbolTable::global_table_->get_local_variable(name); }

SymbolTable::Function& SymbolTable::get_global_function(std::string name)
{ return SymbolTable::global_table_->get_local_function(name); }

SymbolTable::Class& SymbolTable::get_global_class(std::string name)
{ return SymbolTable::global_table_->get_local_class(name); }

void SymbolTable::make_table_global()
{
	for (auto& var : vars_)
		SymbolTable::global_table_->vars_.insert(var);
	for (auto& func : funcs_)
		SymbolTable::global_table_->funcs_.insert(func);
	for (auto& class_ : classes_)
		SymbolTable::global_table_->classes_.insert(class_);
}
