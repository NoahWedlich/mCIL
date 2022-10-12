#include "Number.h"

CIL::Number::Number(double value, bool is_const)
	: Value(cilType(Type::NUM, is_const)), value_(value)
{
}

value_ptr CIL::Number::create(double value, bool is_const)
{
	return value_ptr(new Number(value, is_const));
}

const double CIL::Number::value() const
{
	return value_;
}

value_ptr CIL::Number::invert()
{
	return CIL::Bool::create(!to_bool());
}

value_ptr CIL::Number::negate()
{
	return CIL::Number::create(-value_);
}

value_ptr CIL::Number::increment()
{
	value_++;
	return CIL::Number::create(value_, type_.is_const);
}

value_ptr CIL::Number::decrement()
{
	value_--;
	return CIL::Number::create(value_, type_.is_const);
}

value_ptr CIL::Number::add(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			value_ + std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type("+", other);
}

value_ptr CIL::Number::subtract(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			value_ - std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type("-", other);
}

value_ptr CIL::Number::multiply(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			value_ * std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type("+", other);
}

value_ptr CIL::Number::divide(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			value_ / std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type("+", other);
}

value_ptr CIL::Number::bitwise_not()
{
	return CIL::Number::create((double)~(__int64)value_);
}

value_ptr CIL::Number::bitwise_and(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			(double)((__int64)value_ & (__int64)std::dynamic_pointer_cast<Number>(other)->value())
		);
	}
	throw binary_op_invalid_type("&", other);
}

value_ptr CIL::Number::bitwise_or(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			(double)((__int64)value_ | (__int64)std::dynamic_pointer_cast<Number>(other)->value())
		);
	}
	throw binary_op_invalid_type("|", other);
}

value_ptr CIL::Number::bitwise_xor(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			(double)((__int64)value_ ^ (__int64)std::dynamic_pointer_cast<Number>(other)->value())
		);
	}
	throw binary_op_invalid_type("^", other);
}

value_ptr CIL::Number::left_bitshift(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			(double)((__int64)value_ << (__int64)std::dynamic_pointer_cast<Number>(other)->value())
		);
	}
	throw binary_op_invalid_type("+", other);
}

value_ptr CIL::Number::right_bitshift(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Number::create(
			(double)((__int64)value_ >> (__int64)std::dynamic_pointer_cast<Number>(other)->value())
		);
	}
	throw binary_op_invalid_type("+", other);
}

value_ptr CIL::Number::assign(value_ptr other)
{
	no_assign_to_const();

	if (other->is_type(Type::NUM))
	{
		value_ = std::dynamic_pointer_cast<Number>(other)->value();
	}
	else
	{
		throw binary_op_invalid_type("=", other);
	}

	return other;
}

value_ptr CIL::Number::equals(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Bool::create(
			value_ == std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type("==", other);
}

value_ptr CIL::Number::not_equals(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Bool::create(
			value_ != std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type("!=", other);
}

value_ptr CIL::Number::greater(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Bool::create(
			value_ > std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type(">", other);
}

value_ptr CIL::Number::less(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Bool::create(
			value_ < std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type("<", other);
}

value_ptr CIL::Number::greater_equals(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Bool::create(
			value_ >= std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type(">=", other);
}

value_ptr CIL::Number::less_equals(value_ptr other)
{
	if (other->is_type(Type::NUM))
	{
		return CIL::Bool::create(
			value_ <= std::dynamic_pointer_cast<Number>(other)->value()
		);
	}
	throw binary_op_invalid_type("<=", other);
}

value_ptr CIL::Number::logical_and(value_ptr other)
{
	return CIL::Bool::create(to_bool() && other->to_bool());
}

value_ptr CIL::Number::logical_or(value_ptr other)
{
	return CIL::Bool::create(to_bool() || other->to_bool());
}

std::string CIL::Number::to_string()
{
	return std::to_string(value_);
}

std::string CIL::Number::to_debug_string()
{
	return ("(num: " + std::to_string(value_) + ")");
}

const bool CIL::Number::to_bool()
{
	return value_ != 0;
}