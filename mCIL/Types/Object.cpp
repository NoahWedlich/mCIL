#include "Object.h"

CIL::Object::Object(Class cls, bool is_const)
	: CIL::Value(Type::make(cls.info.name, (is_const ? TypeFlags::CONST : 0))), cls_(cls), env_(new Environment())
{
	for (Function method : cls_.methods)
	{
		env_->define_func(method);
	}

	for (Variable member : cls_.members)
	{
		env_->define_var(member);
	}
}

value_ptr CIL::Object::create(Class info, bool is_const)
{
	return value_ptr(new Object(info, is_const));
}

const Class CIL::Object::cls() const
{ return cls_; }

Environment* CIL::Object::env() const
{ return env_; }

std::string CIL::Object::to_string()
{
	return cls_.info.name;
}

std::string CIL::Object::to_debug_string()
{
	return "Object( " + cls_.info.name + " )";
}

const bool CIL::Object::to_bool()
{
	return true;
}
