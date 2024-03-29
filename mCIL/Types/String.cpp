#include "String.h"

CIL::String::String(std::string value, bool is_const)
	: CIL::Value(Type::make("str", (is_const ? TypeFlags::CONST : 0))), value_(value)
{
}

value_ptr CIL::String::create(std::string value, bool is_const)
{
	return value_ptr(new CIL::String(value, is_const));
}

const std::string CIL::String::value() const
{
	return value_;
}

value_ptr CIL::String::add(value_ptr other)
{
	if (other->is_type(type_))
	{
		return CIL::String::create(
			value_ + std::dynamic_pointer_cast<String>(other)->value()
		);
	}
	throw binary_op_invalid_type("+", other);
}

value_ptr CIL::String::equals(value_ptr other)
{
	if (other->is_type(type_))
	{
		return CIL::Bool::create(
			value_ == std::dynamic_pointer_cast<String>(other)->value()
		);
	}
	throw binary_op_invalid_type("==", other);
}

value_ptr CIL::String::not_equals(value_ptr other)
{
	if (other->is_type(type_))
	{
		return CIL::Bool::create(
			value_ != std::dynamic_pointer_cast<String>(other)->value()
		);
	}
	throw binary_op_invalid_type("!=", other);
}

std::string CIL::String::to_string()
{
	if(value_ == "\\n")
	{ return "\n"; }
	return value_;
}

std::string CIL::String::to_debug_string()
{
	return ("(str: " + value_ + ")");
}

const bool CIL::String::to_bool()
{
	return value_ != "";
}
