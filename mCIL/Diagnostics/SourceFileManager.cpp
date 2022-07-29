#include "SourceFileManager.h"

SourceFileManager::SourceFileManager(std::string file_path)
    : line_buffer_("")
{
    this->file_name_ = file_path.substr(0, file_path.find("."));
    this->file_ = std::ifstream(file_path);

	if (!this->file_.good())
	{
		std::cout << "ERROR: Could not open file '" + file_path + "'" << std::endl;
		exit(1);
	}
}

SourceFileManager::~SourceFileManager()
{
    this->file_.close();
}

void SourceFileManager::update_line_buffer()
{
	if (this->line_buffer_.empty())
	{
		this->line_buffer_ = "";
		char c;
		while (this->file_.get(c))
		{
			this->line_buffer_.push_back(c);
			if (c == '\n')
			{
				break;
			}
		}
	}
}

std::string SourceFileManager::get_line_at_off(size_t line_off)
{
	this->line_buffer_.clear();
	this->file_.clear();
	this->file_.seekg(0, this->file_.beg);

	if (line_off == 0)
	{
		this->update_line_buffer();
		return this->line_buffer_;
	}
	int line = 0;
	char c;
	while (this->file_.get(c))
	{
		if (c == '\n')
		{ line++; }

		if (line == line_off)
		{ break; }
	}
	this->update_line_buffer();

	return this->line_buffer_;
}

bool SourceFileManager::get_next_line(char* line_buffer, size_t max_size, size_t& new_size)
{
	this->update_line_buffer();

	if (this->line_buffer_.size() >= max_size)
	{ return false; }

	strcpy_s(line_buffer, max_size, this->line_buffer_.c_str());
	new_size = this->line_buffer_.size();

	this->line_buffer_.clear();

	return true;
}

bool SourceFileManager::is_at_end()
{
	return this->file_.peek() == EOF;
}
