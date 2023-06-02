#include "SymbolTable.h"

ThreadSafeObj<SymbolTable> SymbolTable::global_table_{};

SymbolTable::SymbolTable()
    : vars_(), funcs_(), classes_()
{
}

void SymbolTable::decl_loc_variable(VarInfo info)
{ vars_.insert({ info.name, info }); }

void SymbolTable::decl_loc_function(FuncInfo info)
{ funcs_.insert({ info.name, info }); }

void SymbolTable::decl_loc_class(ClassInfo info)
{ classes_.insert({ info.name, info }); }

void SymbolTable::decl_glob_variable(VarInfo info)
{ global_table_->decl_loc_variable(info); }

void SymbolTable::decl_glob_function(FuncInfo info)
{ global_table_->decl_loc_function(info); }

void SymbolTable::decl_glob_class(ClassInfo info)
{ global_table_->decl_loc_class(info); }

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

const VarInfo SymbolTable::get_loc_variable(std::string name) const
{
    return vars_.at(name);
}

const FuncInfo SymbolTable::get_loc_function(std::string name) const
{
    return funcs_.at(name);
}

const ClassInfo SymbolTable::get_loc_class(std::string name) const
{
    return classes_.at(name);
}

const VarInfo SymbolTable::get_glob_variable(std::string name)
{
    return SymbolTable::global_table_->vars_.at(name);
}

const FuncInfo SymbolTable::get_glob_function(std::string name)
{
    return SymbolTable::global_table_->funcs_.at(name);
}

const ClassInfo SymbolTable::get_glob_class(std::string name)
{
    return SymbolTable::global_table_->classes_.at(name);
}

void SymbolTable::make_table_global(SymbolTable& table)
{
    for (auto& var : table.vars_)
		SymbolTable::global_table_->decl_glob_variable(var.second);
	for (auto& func : table.funcs_)
		SymbolTable::global_table_->decl_glob_function(func.second);
	for (auto& class_ : table.classes_)
		SymbolTable::global_table_->decl_glob_class(class_.second);
}
