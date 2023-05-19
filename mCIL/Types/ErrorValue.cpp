#include "ErrorValue.h"

CIL::ErrorValue::ErrorValue()
    : CIL::Value(Type::make("error", true))
{
}

value_ptr CIL::ErrorValue::create()
{
    return value_ptr(new ErrorValue());
}

std::string CIL::ErrorValue::to_string()
{
    return "ERROR";
}

std::string CIL::ErrorValue::to_debug_string()
{
    return ("Error");
}

const bool CIL::ErrorValue::to_bool()
{
    return false;
}
