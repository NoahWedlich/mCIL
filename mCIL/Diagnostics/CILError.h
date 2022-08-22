#pragma once
#include "../cil-system.h"
#include "../Types/cil-types.h"
#include "Position.h"
#include "TokenPrinter.h"
#include "../Interpreting/Object.h"
#include "../Lexing/Token.h"

class CILError : std::exception
{
public:
	CILError(const std::string& msg, Position range, Position focus, bool has_pos = true);

	template <typename... Targs>
	static CILError error(const std::string fmt, Targs... Fargs)
	{
		const std::string msg = CILError::format_msg(fmt.c_str(), Fargs...);
		return CILError(msg, Position(0, 0), Position(0, 0), false);
	}

	template <typename... Targs>
	static CILError error(Position range, const std::string fmt, Targs... Fargs)
	{
		const std::string msg = CILError::format_msg(fmt.c_str(), Fargs...);
		return CILError(msg, range, range);
	}

	template <typename... Targs>
	static CILError error(Position range, Position focus, const std::string fmt, Targs... Fargs)
	{
		const std::string msg = CILError::format_msg(fmt.c_str(), Fargs...);
		return CILError(msg, range, focus);
	}

	const char* what() const override;
	const Position range() const;
	const Position focus() const;
	bool has_pos() const;

	void add_range(Position range);
	void add_focus(Position focus);

private:
	static void insert(std::stringstream& ss, int value);
	static void insert(std::stringstream& ss, double value);
	static void insert(std::stringstream& ss, char value);
	static void insert(std::stringstream& ss, size_t value);
	static void insert(std::stringstream& ss, std::string value);
	static void insert(std::stringstream& ss, cilType value);
	static void insert(std::stringstream& ss, TokenType value);
	static void insert(std::stringstream& ss, Symbol value);
	static void insert(std::stringstream& ss, Operator value);
	static void insert(std::stringstream& ss, Keyword value);

	static const std::string format_msg(const char* fmt)
	{
		std::stringstream ss;
		for (; *fmt != '\0'; fmt++)
		{
			ss << *fmt;
		}
		return ss.str();
	}

	template<typename T, typename... Targs>
	static const std::string format_msg(const char* fmt, T value, Targs... Fargs)
	{
		std::stringstream ss;
		for (; *fmt != '\0'; fmt++)
		{
			if (*fmt != '$')
			{
				ss << *fmt;
				continue;
			}
			CILError::insert(ss, value);
			ss << CILError::format_msg(++fmt, Fargs...);
			break;
		}
		return ss.str();
	}

	const std::string msg_;
	bool has_pos_;
	Position range_;
	Position focus_;
};