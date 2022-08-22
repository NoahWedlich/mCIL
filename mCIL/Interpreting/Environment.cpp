#include "Environment.h"

Environment::Environment()
	: variables_(), arrays_(), functions_(), enclosing_(nullptr)
{
}

Environment::Environment(Environment* enclosing)
	: variables_(), arrays_(), functions_(), enclosing_(enclosing)
{
}

Environment::~Environment()
{
}

void Environment::define_var(Variable var)
{
	if (this->variables_.contains(var.info.name))
	{
		throw CILError::error("Redifinition of variable '$'", var.info.name.c_str());
	}
	this->variables_.insert({ var.info.name, var });
}

void Environment::assign_var(const std::string name, Object value)
{
	if (this->var_exists(name))
	{
		if (value.type() != this->get_var(name).info.type)
		{
			throw CILError::error("Cannot assign value of type '$' to variable '$' of type '$'", 
				value.type(), name.c_str(), this->get_var(name).info.type);
		}
		if (this->get_var(name).info.type.is_const)
		{
			throw CILError::error("Cannot assign to const variable '$'", name.c_str());
		}
		this->variables_.at(name).value = value;
	}
	else if (this->enclosing_ && this->enclosing_->var_exists(name))
	{
		this->enclosing_->assign_var(name, value);
	}
	else
	{
		throw CILError::error("Assigning to undefined variable '$'", name.c_str());
	}
}

void Environment::define_arr(Array arr)
{
	if (this->arrays_.contains(arr.info.name))
	{
		throw CILError::error("Redifinition of array '$'", arr.info.name.c_str());
	}
	this->arrays_.insert({ arr.info.name, arr });
}

void Environment::define_func(Function func)
{
	if (this->func_exists(func))
	{
		throw CILError::error("Redifinition of function '$'", func.info.name.c_str());
	}
	this->functions_.insert({ func.info.name, func });
}

Variable Environment::get_var(const std::string name)
{
	if (this->variables_.contains(name))
	{
		return this->variables_.at(name);
	}
	else if (this->enclosing_ && this->enclosing_->var_exists(name))
	{
		return this->enclosing_->get_var(name);
	}
	else
	{
		throw CILError::error("Undefined variable '$'", name.c_str());
	}
}

Array Environment::get_arr(const std::string name)
{
	if (this->arrays_.contains(name))
	{
		return this->arrays_.at(name);
	}
	else if (this->enclosing_ && this->enclosing_->arr_exists(name))
	{
		return this->enclosing_->get_arr(name);
	}
	else
	{
		throw CILError::error("Undefined array '$'", name.c_str());
	}
}

Function Environment::get_func(const std::string name)
{
	if (this->functions_.contains(name))
	{
		return this->functions_.at(name);
	}
	else if (this->enclosing_ && this->enclosing_->func_exists(name))
	{
		return this->enclosing_->get_func(name);
	}
	else
	{
		throw CILError::error("Undefined function '$'", name.c_str());
	}
}

bool Environment::var_exists(const std::string name)
{
	if (this->variables_.contains(name))
	{ return true; }
	if (this->enclosing_ != nullptr)
	{ return this->enclosing_->var_exists(name); }
	else
	{ return false; }
}

bool Environment::var_exists(Variable var)
{
	if (this->variables_.contains(var.info.name))
	{ return true; }
	if (this->enclosing_ != nullptr)
	{ return this->enclosing_->var_exists(var); }
	else
	{ return false; }
}

bool Environment::arr_exists(const std::string name)
{
	if (this->arrays_.contains(name))
	{ return true; }
	if (this->enclosing_ != nullptr)
	{ return this->enclosing_->arr_exists(name); }
	else
	{ return false; }
}

bool Environment::arr_exists(Array arr)
{
	return this->arr_exists(arr.info.name);
}

bool Environment::func_exists(const std::string name)
{
	if (this->functions_.contains(name))
	{ return true; }
	if (this->enclosing_ != nullptr)
	{ return this->enclosing_->func_exists(name); }
	else
	{ return false; }
}

bool Environment::func_exists(Function func)
{
	if (this->functions_.contains(func.info.name))
	{ return true; }
	if (this->enclosing_ != nullptr)
	{ return this->enclosing_->func_exists(func); }
	else
	{ return false; }
}
