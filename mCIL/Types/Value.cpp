#include "Value.h"

value_ptr CIL::Value::invert()
{
	throw unary_op_not_implemented("!");
}

value_ptr CIL::Value::negate()
{
	throw unary_op_not_implemented("-");
}

value_ptr CIL::Value::increment()
{
	throw unary_op_not_implemented("++");
}

value_ptr CIL::Value::decrement()
{
	throw unary_op_not_implemented("--");
}

value_ptr CIL::Value::add(value_ptr other)
{
	throw binary_op_not_implemented("+", other);
}

value_ptr CIL::Value::subtract(value_ptr other)
{
	throw binary_op_not_implemented("-", other);
}

value_ptr CIL::Value::multiply(value_ptr other)
{
	throw binary_op_not_implemented("*", other);
}

value_ptr CIL::Value::divide(value_ptr other)
{
	throw binary_op_not_implemented("/", other);
}

value_ptr CIL::Value::left_bitshift(value_ptr other)
{
	throw binary_op_not_implemented("<<", other);
}

value_ptr CIL::Value::right_bitshift(value_ptr other)
{
	throw binary_op_not_implemented(">>", other);
}

value_ptr CIL::Value::assign(value_ptr other)
{
	throw binary_op_not_implemented("=", other);
}

value_ptr CIL::Value::equals(value_ptr other)
{
	throw binary_op_not_implemented("==", other);
}

value_ptr CIL::Value::not_equals(value_ptr other)
{
	throw binary_op_not_implemented("!=", other);
}

value_ptr CIL::Value::greater(value_ptr other)
{
	throw binary_op_not_implemented(">", other);
}

value_ptr CIL::Value::less(value_ptr other)
{
	throw binary_op_not_implemented("<", other);
}

value_ptr CIL::Value::greater_equals(value_ptr other)
{
	throw binary_op_not_implemented(">=", other);
}

value_ptr CIL::Value::less_equals(value_ptr other)
{
	throw binary_op_not_implemented("<=", other);
}

value_ptr CIL::Value::logical_and(value_ptr other)
{
	throw binary_op_not_implemented("&&", other);
}

value_ptr CIL::Value::logical_or(value_ptr other)
{
	throw binary_op_not_implemented("||", other);
}

CILError CIL::Value::unary_op_not_implemented(std::string op)
{
	return CILError::error("Operator '$' is not implemented for type '$'", op, type());
}

CILError CIL::Value::binary_op_not_implemented(std::string op, value_ptr val)
{
	return CILError::error("Operator '$' is not implemented for types '$' and '$'", op, type(), val->type());
}

CILError CIL::Value::binary_op_invalid_type(std::string op, value_ptr val)
{
	return CILError::error("Invalid operands '$' and '$' for operator '$'", type(), val->type(), op);
}

void CIL::Value::no_assign_to_const()
{
	if (type_.is_const)
	{
		throw CILError::error("Cannot assign to const '$'", type());
	}
}