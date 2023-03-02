#pragma once
#include "Value.h"

namespace CIL
{
	class Bool : public Value
	{
	public:
		static value_ptr create(bool value, bool is_const = false);

		const bool value() const;

		virtual value_ptr invert() override;

		virtual value_ptr equals(value_ptr) override;
		virtual value_ptr not_equals(value_ptr) override;
		virtual value_ptr logical_and(value_ptr) override;
		virtual value_ptr logical_or(value_ptr) override;

		virtual std::string to_string() override;
		virtual std::string to_debug_string() override;

		virtual const bool to_bool() override;
	private:
		Bool(bool value, bool is_const);

		bool value_;
	};
}

