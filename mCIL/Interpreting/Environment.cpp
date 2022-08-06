#include "Environment.h"

Environment::Environment()
	: variables_(), enclosing_(nullptr) {}

Environment::Environment(Environment* enclosing)
	: variables_(), enclosing_(enclosing) {}

Environment::~Environment()
{
}

void Environment::define(const std::string& name, Object value)
{
	if (this->exists(name))
	{
		throw CIL_Error("Trying to create variable that already exists");
	}
	this->variables_.insert({ name, value });
}

void Environment::assign(const std::string& name, Object value)
{
	if (this->variables_.contains(name))
	{
		this->variables_.at(name) = value;
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

Object Environment::get(const std::string& name)
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
