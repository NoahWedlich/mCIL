#include "None.h"

CIL::None::None()
    : CIL::Value(Type::make("none", true))
{
}

value_ptr CIL::None::create()
{
    return value_ptr(new None());
}

std::string CIL::None::to_string()
{
    return "NONE";
}

std::string CIL::None::to_debug_string()
{
    return ("None");
}

const bool CIL::None::to_bool()
{
    return false;
}
