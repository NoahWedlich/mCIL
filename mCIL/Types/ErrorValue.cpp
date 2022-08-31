#include "ErrorValue.h"

value_ptr CIL::ValueError::create()
{
    return value_ptr(new ValueError());
}

std::string CIL::ValueError::to_string()
{
    return "ERROR";
}

std::string CIL::ValueError::to_debug_string()
{
    return ("Error");
}

const bool CIL::ValueError::to_bool()
{
    return false;
}
