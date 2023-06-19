#include "Object.h"

CIL::Object::Object(Environment::Class cls, bool is_const)
	: CIL::Value(Type::make(cls.name, (is_const ? TypeFlags::CONST : 0))), cls_(cls), env_(new Environment())
{
	for (Environment::Function method : cls_.methods)
	{
		env_->define_func(method);
	}

	for (Environment::Variable member : cls_.members)
	{
		env_->define_var(member);
	}
}

value_ptr CIL::Object::create(Environment::Class info, bool is_const)
{
	return value_ptr(new Object(info, is_const));
}

const Environment::Class CIL::Object::cls() const
{ return cls_; }

Environment* CIL::Object::env() const
{ return env_; }

std::string CIL::Object::to_string()
{
	return cls_.name;
}

std::string CIL::Object::to_debug_string()
{
	return "Object( " + cls_.name + " )";
}

const bool CIL::Object::to_bool()
{
	return true;
}
