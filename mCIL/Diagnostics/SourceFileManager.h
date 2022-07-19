#pragma once
#include "../cil-system.h"
#include "SourceManager.h"

class SourceFileManager : public SourceManager
{
public:
	SourceFileManager(std::string file_path);
	~SourceFileManager();

	virtual bool get_next_line(char* line_buffer, size_t max_size, size_t& new_size) override;
	virtual std::string get_line_at_off(size_t line_off) override;
	virtual bool is_at_end() override;

private:
	void update_line_buffer();

	std::string file_name_;
	std::string line_buffer_;
	std::ifstream file_;
};

