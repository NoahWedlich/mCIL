#include "String.h"

CIL::String::String(std::string value, bool is_const)
	: CIL::Value(cilType(Type::STR, is_const)), value_(value)
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
	if (other->is_type(Type::STR))
	{
		return CIL::String::create(
			value_ + std::dynamic_pointer_cast<String>(other)->value()
		);
	}
	throw binary_op_invalid_type("+", other);
}

value_ptr CIL::String::assign(value_ptr other)
{
	no_assign_to_const();

	if (other->is_type(Type::STR))
	{
	value_ = std::dynamic_pointer_cast<String>(other)->value();
	}
	else
	{
		throw binary_op_invalid_type("=", other);
	}

	return other;
}

value_ptr CIL::String::equals(value_ptr other)
{
	if (other->is_type(Type::STR))
	{
		return CIL::Bool::create(
			value_ == std::dynamic_pointer_cast<String>(other)->value()
		);
	}
	throw binary_op_invalid_type("==", other);
}

value_ptr CIL::String::not_equals(value_ptr other)
{
	if (other->is_type(Type::STR))
	{
		return CIL::Bool::create(
			value_ != std::dynamic_pointer_cast<String>(other)->value()
		);
	}
	throw binary_op_invalid_type("!=", other);
}

std::string CIL::String::to_string()
{
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
