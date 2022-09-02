#pragma once
#include "Value.h"
#include "Bool.h"

namespace CIL
{
	class Number : public Value
	{
	public:
		static value_ptr create(double value, bool is_const = false);

		const double value() const;

		virtual value_ptr invert() override;
		virtual value_ptr negate() override;
		virtual value_ptr increment() override;
		virtual value_ptr decrement() override;

		virtual value_ptr add(value_ptr) override;
		virtual value_ptr subtract(value_ptr) override;
		virtual value_ptr multiply(value_ptr) override;
		virtual value_ptr divide(value_ptr) override;

		virtual value_ptr assign(value_ptr) override;

		virtual value_ptr equals(value_ptr) override;
		virtual value_ptr not_equals(value_ptr) override;
		virtual value_ptr greater(value_ptr) override;
		virtual value_ptr less(value_ptr) override;
		virtual value_ptr greater_equals(value_ptr) override;
		virtual value_ptr less_equals(value_ptr) override;
		virtual value_ptr logical_and(value_ptr) override;
		virtual value_ptr logical_or(value_ptr) override;

		virtual std::string to_string() override;
		virtual std::string to_debug_string() override;

		virtual const bool to_bool() override;
	private:
		Number(double value, bool is_const);

		double value_;
	};
}

