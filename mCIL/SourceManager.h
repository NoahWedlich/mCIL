#pragma once
#include "cil-system.h"

class SourceManager
{
public:
	virtual bool get_next_line(char* line_buffer, size_t max_size, size_t& new_size) = 0;
	virtual std::string get_line_at_off(size_t line_off) = 0;
	virtual bool is_at_end() = 0;
};