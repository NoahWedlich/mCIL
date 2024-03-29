#include "Environment.h"

Environment::Environment()
	: variables_(), arrays_(), functions_(), classes_(), enclosing_(nullptr)
{
}

Environment::Environment(Environment* enclosing)
	: variables_(), arrays_(), functions_(), classes_(), enclosing_(enclosing)
{
}

Environment::~Environment()
{
}

void Environment::define_var(Variable var)
{
	if (this->variables_.contains(var.name))
	{
		throw CILError::error("Redifinition of variable '$'", var.name.c_str());
	}
	this->variables_.insert({ var.name, var });
}

void Environment::define_arr(Array arr)
{
	if (this->arrays_.contains(arr.name))
	{
		throw CILError::error("Redifinition of array '$'", arr.name.c_str());
	}
	this->arrays_.insert({ arr.name, arr });
}

void Environment::define_func(Function func)
{
	if (this->functions_.contains(func.name))
	{
		if (functions_.at(func.name).body)
		{
			throw CILError::error("Redifinition of function '$'", func.name.c_str());
		}
		functions_.at(func.name) = func;
	}
	this->functions_.insert({ func.name, func });
}

void Environment::define_class(Class cls)
{
	if (classes_.contains(cls.name))
	{
		throw CILError::error("Redifinition of class '$'", cls.name.c_str());
	}
	classes_.insert({ cls.name, cls });
}

Environment::Variable& Environment::get_var(const std::string name)
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

Environment::Array& Environment::get_arr(const std::string name)
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

Environment::Function& Environment::get_func(const std::string name)
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

Environment::Class& Environment::get_class(const std::string name)
{
	if (classes_.contains(name))
	{
		return classes_.at(name);
	}
	else if (enclosing_ && enclosing_->class_exists(name))
	{
		return enclosing_->get_class(name);
	}
	else
	{
		throw CILError::error("Undefined class '$'", name.c_str());
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
	if (this->variables_.contains(var.name))
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
	return this->arr_exists(arr.name);
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
	if (this->functions_.contains(func.name))
	{ return true; }
	if (this->enclosing_ != nullptr)
	{ return this->enclosing_->func_exists(func); }
	else
	{ return false; }
}

bool Environment::class_exists(const std::string name)
{
	if (classes_.contains(name))
	{ return true; }
	if (enclosing_ != nullptr)
	{ return enclosing_->class_exists(name); }
	else
	{ return false; }
}

bool Environment::class_exists(Class cls)
{
	return class_exists(cls.name);
}

void Environment::add_enclosing(Environment* other)
{ enclosing_ = other; }

void Environment::rem_enclosing()
{ enclosing_ = nullptr; }
