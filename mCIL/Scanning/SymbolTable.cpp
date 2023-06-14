#include "SymbolTable.h"

ThreadSafeObj<SymbolTable> SymbolTable::global_table_{};

SymbolTable::SymbolTable()
    : vars_(), funcs_(), classes_()
{
}

void SymbolTable::decl_loc_variable(VarInfo info, token_list tokens)
{
    if (!vars_.contains(info.name))
        vars_.insert({ info.name, {info, tokens} });
    else if (vars_.at(info.name).tokens_.empty())
		vars_.at(info.name).tokens_ = tokens;
	else
		throw CILError::error("Variable $ already defined", info.name);
}

void SymbolTable::decl_loc_function(FuncInfo info, token_list tokens)
{
    if (!funcs_.contains(info.name))
		funcs_.insert({ info.name, {info, tokens} });
	else if (funcs_.at(info.name).tokens_.empty())
		funcs_.at(info.name).tokens_ = tokens;
	else
		throw CILError::error("Function $ already defined", info.name);
}

void SymbolTable::decl_loc_class(ClassInfo info, token_list tokens)
{
    if (!classes_.contains(info.name))
        classes_.insert({ info.name, {info, tokens} });
    else if (classes_.at(info.name).tokens_.empty())
        classes_.at(info.name).tokens_ = tokens;
    else
        throw CILError::error("Class $ already defined", info.name);
}

void SymbolTable::decl_glob_variable(VarInfo info, token_list tokens)
{ global_table_->decl_loc_variable(info, tokens); }

void SymbolTable::decl_glob_function(FuncInfo info, token_list tokens)
{ global_table_->decl_loc_function(info, tokens); }

void SymbolTable::decl_glob_class(ClassInfo info, token_list tokens)
{ global_table_->decl_loc_class(info, tokens); }

const bool SymbolTable::exists_loc_variable(std::string name) const
{
    return vars_.contains(name);
}

const bool SymbolTable::exists_loc_function(std::string name) const
{
    return funcs_.contains(name);
}

const bool SymbolTable::exists_loc_class(std::string name) const
{
    return classes_.contains(name);
}

const bool SymbolTable::exists_glob_variable(std::string name)
{
    return SymbolTable::global_table_->vars_.contains(name);
}

const bool SymbolTable::exists_glob_function(std::string name)
{
    return SymbolTable::global_table_->funcs_.contains(name);
}

const bool SymbolTable::exists_glob_class(std::string name)
{
    return SymbolTable::global_table_->classes_.contains(name);
}

//TODO: Add getter for the definitions in the symbol table
const VarInfo SymbolTable::get_loc_variable(std::string name) const
{
    return vars_.at(name).info;
}

const FuncInfo SymbolTable::get_loc_function(std::string name) const
{
    return funcs_.at(name).info;
}

const ClassInfo SymbolTable::get_loc_class(std::string name) const
{
    return classes_.at(name).info;
}

const VarInfo SymbolTable::get_glob_variable(std::string name)
{
    return SymbolTable::global_table_->get_loc_variable(name);
}

const FuncInfo SymbolTable::get_glob_function(std::string name)
{
    return SymbolTable::global_table_->get_loc_function(name);
}

const ClassInfo SymbolTable::get_glob_class(std::string name)
{
    return SymbolTable::global_table_->get_loc_class(name);
}

void SymbolTable::make_table_global(SymbolTable& table)
{
    for (auto& var : table.vars_)
		SymbolTable::global_table_->decl_glob_variable(var.second.info, var.second.tokens_);
	for (auto& func : table.funcs_)
		SymbolTable::global_table_->decl_glob_function(func.second.info, func.second.tokens_);
	for (auto& class_ : table.classes_)
		SymbolTable::global_table_->decl_glob_class(class_.second.info, class_.second.tokens_);
}
