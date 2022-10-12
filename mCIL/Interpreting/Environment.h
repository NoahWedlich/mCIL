#pragma once
#include "../cil-system.h"
#include "../Types/RuntimeObjects.h"
#include "../Diagnostics/CILError.h"
#include "../Types/Value.h"

class Environment
{
public:
	Environment();
	Environment(Environment* enclosing);

	~Environment();
	
	void define_var(Variable var);

	void define_arr(Array arr);

	void define_func(Function func);

	void define_class(Class cls);

	Variable& get_var(const std::string name);
	Array& get_arr(const std::string name);
	Function& get_func(const std::string name);
	Class& get_class(const std::string name);

	bool var_exists(const std::string name);
	bool var_exists(Variable var);

	bool arr_exists(const std::string name);
	bool arr_exists(Array arr);

	bool func_exists(const std::string name);
	bool func_exists(Function func);

	bool class_exists(const std::string name);
	bool class_exists(Class cls);

	void add_enclosing(Environment* other);
	void rem_enclosing();

	bool has_enclosing()
	{ return this->enclosing_ != nullptr; }
private:
	std::map<const std::string, Variable> variables_;
	std::map<const std::string, Array> arrays_;
	std::map<const std::string, Function> functions_;
	std::map<const std::string, Class> classes_;

	Environment* enclosing_;
};

