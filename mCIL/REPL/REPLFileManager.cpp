#include "REPLFileManager.h"

REPLFileManager::REPLFileManager()
	: lines_(0), curr_line_(0)
{
	this->source_ = *new std::vector<std::string>();
}

REPLFileManager::~REPLFileManager()
{
	delete &this->source_;
}

void REPLFileManager::add_line(const std::string& line)
{
	this->source_.push_back(line);
	this->lines_++;
}

void REPLFileManager::clear()
{
	this->source_.clear();
	this->lines_ = 0;
	this->curr_line_ = 0;
}

bool REPLFileManager::get_next_line(char* line_buffer, size_t max_size, size_t& new_size)
{
	if (this->curr_line_ < this->lines_ || this->source_[this->curr_line_].size() < max_size)
	{
		strcpy_s(line_buffer, max_size, this->source_[this->curr_line_].c_str());
		new_size = this->source_[this->curr_line_].size();
		this->curr_line_++;
		return true;
	}
	throw (new LexerError("Error reading next line", Position(this->curr_line_, 0)));
}

std::string REPLFileManager::get_line_at_off(size_t line_off)
{
	if (line_off < this->lines_)
	{
		return this->source_[line_off];
	}
	throw (new LexerError("Invalid line offset", Position(0, 0)));
}

bool REPLFileManager::is_at_end()
{
	return this->curr_line_ == this->lines_;
}
