#pragma once
#include "Value.h"

namespace CIL
{
	class ErrorValue : public Value
	{
	public:
		static value_ptr create();

		virtual std::string to_string() override;
		virtual std::string to_debug_string() override;
		virtual const bool to_bool() override;
	private:
		ErrorValue();
	};
}
