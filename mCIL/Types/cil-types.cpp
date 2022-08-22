#include "cil-types.h"

bool operator==(cilType a, cilType b)
{
	return (a.is_const == b.is_const && a.type == b.type);
}

bool operator!=(cilType a, cilType b)
{
	return !(a.is_const == b.is_const && a.type == b.type);
}
