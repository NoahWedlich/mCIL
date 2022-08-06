#pragma once
#include "../cil-system.h"
#include "../Diagnostics/Errors.h"
#include "Object.h"

class Environment
{
public:
	Environment();
	Environment(Environment* enclosing);

	~Environment();
	
	void define(const std::string& name, Object value);
	void assign(const std::string& name, Object value);
	Object get(const std::string& name);
	bool exists(const std::string& name);

	bool has_enclosing()
	{ return this->enclosing_ != nullptr; }
private:
	std::map<const std::string, Object> variables_;

	Environment* enclosing_;
};

