#pragma once
#include "../cil-system.h"
#include "Type.h"
#include "../Diagnostics/CILError.h"

namespace CIL
{ class Value; }

typedef std::shared_ptr<CIL::Value> value_ptr;

namespace CIL
{
	class Value
	{
	public:
		const Type type() const
		{ return type_; }

		const bool is_type(Type t) const
		{ return type_.is(t); }

		virtual value_ptr invert();
		virtual value_ptr negate();
		virtual value_ptr increment();
		virtual value_ptr decrement();

		virtual value_ptr add(value_ptr);
		virtual value_ptr subtract(value_ptr);
		virtual value_ptr multiply(value_ptr);
		virtual value_ptr divide(value_ptr);

		virtual value_ptr bitwise_not();
		virtual value_ptr bitwise_and(value_ptr);
		virtual value_ptr bitwise_or(value_ptr);
		virtual value_ptr bitwise_xor(value_ptr);

		virtual value_ptr left_bitshift(value_ptr);
		virtual value_ptr right_bitshift(value_ptr);

		virtual value_ptr equals(value_ptr);
		virtual value_ptr not_equals(value_ptr);
		virtual value_ptr greater(value_ptr);
		virtual value_ptr less(value_ptr);
		virtual value_ptr greater_equals(value_ptr);
		virtual value_ptr less_equals(value_ptr);
		virtual value_ptr logical_and(value_ptr);
		virtual value_ptr logical_or(value_ptr);

		virtual std::string to_string() = 0;
		virtual std::string to_debug_string() = 0;

		virtual const bool to_bool() = 0;
	protected:
		Value(Type type)
			: type_(type) {}

		const Type type_;

		CILError unary_op_not_implemented(std::string op);

		CILError binary_op_not_implemented(std::string op, value_ptr val);

		CILError binary_op_invalid_type(std::string op, value_ptr val);
	};
}