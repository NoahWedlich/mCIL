#pragma once
#include "Value.h"
#include "../Interpreting/Environment.h"

namespace CIL
{
	class Object : public Value
	{
	public:
		static value_ptr create(Environment::Class cls, bool is_const = false);

		const Environment::Class cls() const;
		Environment* env() const;

		virtual std::string to_string() override;
		virtual std::string to_debug_string() override;
		virtual const bool to_bool() override;
	private:
		Object(Environment::Class cls, bool is_const);

		Environment::Class cls_;
		Environment* env_;
	};
}

