#pragma once
#include "Value.h"
#include "Bool.h"

namespace CIL
{
	class String : public Value
	{
	public:
		static value_ptr create(std::string value, bool is_const = false);

		const std::string value() const;

		virtual value_ptr add(value_ptr) override;

		virtual value_ptr equals(value_ptr) override;
		virtual value_ptr not_equals(value_ptr) override;

		virtual std::string to_string() override;
		virtual std::string to_debug_string() override;

		virtual const bool to_bool() override;
	private:
		String(std::string value, bool is_const);

		std::string value_;
	};
}
