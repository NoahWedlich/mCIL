#include "Environment.h"

Environment::Environment()
	: variables_(), enclosing_(nullptr) {}

Environment::Environment(Environment* enclosing)
	: variables_(), enclosing_(enclosing) {}

Environment::~Environment()
{
}

void Environment::define(Variable var)
{
	if (this->exists(var))
	{
		CILError::error("Redifinition of variable '$'", var.name.c_str());
	}
	this->variables_.insert({ var.name, var });
}

void Environment::assign(const std::string name, Object value)
{
	if (this->exists(name))
	{
		if (value.type() != this->get(name).type)
		{
			CILError::error("Cannot assign value of type '$' to variable '$' of type '$'", 
				value.type(), name.c_str(), this->get(name).type);
		}
		if (this->get(name).is_const)
		{
			CILError::error("Cannot assign to const variable '$'", name.c_str());
		}
		this->variables_.at(name).value = value;
	}
	else if (this->enclosing_ && this->enclosing_->exists(name))
	{
		this->enclosing_->assign(name, value);
	}
	else
	{
		CILError::error("Assigning to undefined variable '$'", name.c_str());
	}
}

Variable Environment::get(const std::string name)
{
	if (this->variables_.contains(name))
	{
		return this->variables_.at(name);
	}
	else if (this->enclosing_ && this->enclosing_->exists(name))
	{
		return this->enclosing_->get(name);
	}
	else
	{
		CILError::error("Undefined variable '$'", name.c_str());
	}
}

bool Environment::exists(const std::string name)
{
	if (this->variables_.contains(name))
	{ return true; }
	if (this->enclosing_ != nullptr)
	{ return this->enclosing_->exists(name); }
	else
	{ return false; }
}

bool Environment::exists(Variable var)
{
	if (this->variables_.contains(var.name))
	{ return true; }
	if (this->enclosing_ != nullptr)
	{ return this->enclosing_->exists(var.name); }
	else
	{ return false; }
}
