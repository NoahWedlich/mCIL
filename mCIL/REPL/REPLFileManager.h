#pragma once
#include "../cil-system.h"
#include "../Diagnostics/SourceManager.h"
#include "../Diagnostics/CILError.h"

class REPLFileManager : public SourceManager
{
public:
	REPLFileManager();
	~REPLFileManager();

	void add_line(const std::string& line);
	void clear();

	virtual bool get_next_line(char* line_buffer, size_t max_size, size_t& new_size) override;
	virtual std::string get_line_at_off(size_t line_off) override;
	virtual bool is_at_end() override;
private:
	size_t lines_;
	size_t curr_line_;
	std::vector<std::string> source_;
};

