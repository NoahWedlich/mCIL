#include "Bool.h"

CIL::Bool::Bool(bool value, bool is_const)
	: CIL::Value(Type::make("bool", (is_const ? TypeFlags::CONST : 0))), value_(value)
{
}

value_ptr CIL::Bool::create(bool value, bool is_const)
{
	return value_ptr(new CIL::Bool(value, is_const));
}

const bool CIL::Bool::value() const
{
	return value_;
}

value_ptr CIL::Bool::invert()
{
	return CIL::Bool::create(!value_, type_.is_const());
}

value_ptr CIL::Bool::equals(value_ptr other)
{
	return CIL::Bool::create(value_ == other->to_bool());
}

value_ptr CIL::Bool::not_equals(value_ptr other)
{
	return CIL::Bool::create(value_ != other->to_bool());
}

value_ptr CIL::Bool::logical_and(value_ptr other)
{
	return CIL::Bool::create(value_ && other->to_bool());
}

value_ptr CIL::Bool::logical_or(value_ptr other)
{
	return CIL::Bool::create(value_ || other->to_bool());
}

std::string CIL::Bool::to_string()
{
	return (value_ ? "true" : "false");
}

std::string CIL::Bool::to_debug_string()
{
	return (value_ ? "(bool: true)" : "(bool: false)");
}

const bool CIL::Bool::to_bool()
{
	return value_;
}
