#pragma once
#include "Value.h"
#include "../Interpreting/Environment.h"

namespace CIL
{
	class Object : public Value
	{
	public:
		static value_ptr create(Class cls, bool is_const = false);

		const Class cls() const;
		Environment* env() const;

		virtual std::string to_string() override;
		virtual std::string to_debug_string() override;
		virtual const bool to_bool() override;
	private:
		Object(Class cls, bool is_const);

		Class cls_;
		Environment* env_;
	};
}

