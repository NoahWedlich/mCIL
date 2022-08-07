#pragma once
#include "../cil-system.h"
#include "../Diagnostics/CILError.h"
#include "Object.h"
#include "Variable.h"

class Environment
{
public:
	Environment();
	Environment(Environment* enclosing);

	~Environment();
	
	void define(Variable var);
	void assign(const std::string name, Object value);

	Variable get(const std::string name);
	//TODO: Implement these
	ObjType get_type(const std::string name);
	bool get_is_const(const std::string name);

	bool exists(const std::string name);
	bool exists(Variable var);

	bool has_enclosing()
	{ return this->enclosing_ != nullptr; }
private:
	std::map<const std::string, Variable> variables_;

	Environment* enclosing_;
};

