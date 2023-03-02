#include "None.h"

CIL::None::None()
	: CIL::Value(cilType(Type::NONE, false)) {}

value_ptr CIL::None::create()
{
	return value_ptr(new CIL::None());
}

std::string CIL::None::to_string()
{
	return std::string("none");
}

std::string CIL::None::to_debug_string()
{
	return std::string("(none)");
}

const bool CIL::None::to_bool()
{
	return false;
}
