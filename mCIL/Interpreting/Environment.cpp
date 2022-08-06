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
		throw CIL_Error("Trying to create variable that already exists");
	}
	this->variables_.insert({ var.name, var });
}

void Environment::assign(const std::string& name, Object value)
{
	if (this->exists(name))
	{
		if (value.type() != this->get(name).type)
		{ throw CIL_Error("Trying to change type of variable"); }
		if (this->get(name).is_const)
		{ throw CIL_Error("Trying to change const variable"); }

		this->variables_.at(name).value = value;
	}
	else if (this->enclosing_ && this->enclosing_->exists(name))
	{
		this->enclosing_->assign(name, value);
	}
	else
	{
		throw CIL_Error("Trying to assign to a variable that doesn't exist");
	}
}

Variable Environment::get(const std::string& name)
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
		throw CIL_Error("Trying to get a variable that doesn't exist");
	}
}

bool Environment::exists(const std::string& name)
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
