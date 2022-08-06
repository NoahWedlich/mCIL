#include "Position.h"

bool SourcePos::operator<(const SourcePos& other) const
{
	if (line_off == other.line_off)
	{
		return char_off < other.char_off;
	}
	return line_off < other.line_off;
}

bool SourcePos::operator>(const SourcePos& other) const
{
	if (line_off == other.line_off)
	{
		return char_off > other.char_off;
	}
	return line_off > other.line_off;
}

bool SourcePos::operator<=(const SourcePos& other) const
{
	if (line_off == other.line_off)
	{
		return char_off <= other.char_off;
	}
	return line_off <= other.line_off;
}

bool SourcePos::operator>=(const SourcePos& other) const
{
	if (line_off == other.line_off)
	{
		return char_off >= other.char_off;
	}
	return line_off >= other.line_off;
}

SourceRange::SourceRange(SourcePos start, SourcePos end)
	: start(start), end(end)
{
	multi_line = start.line_off != end.line_off;
	if (start > end)
	{
		SourcePos temp = start;
		end = start;
		start = temp;
	}
}

bool SourceRange::includes(SourceRange other)
{
	return start <= other.start && end >= other.start;
}

Position::Position(size_t line_off, size_t char_off)
{
	SourcePos pos{ line_off, char_off };
	range_ = SourceRange{ pos, pos };
	focus_ = SourceRange{ pos, pos };
	if (!range_.includes(focus_))
	{ throw std::invalid_argument("Focus is outside of range"); }
}

Position::Position(const SourcePos& pos)
	: range_(pos, pos), focus_(pos, pos) {}

Position::Position(const SourcePos& start, const SourcePos& end)
	: range_(start, end), focus_(start, end) {}

Position::Position(const SourceRange& range)
	: range_(range), focus_(range) {}

Position::Position(const SourceRange& range, const SourceRange& focus)
	: range_(range), focus_(focus) {}

const SourceRange Position::range() const
{
	return this->range_;
}

const SourceRange Position::focus() const
{
	return this->focus_;
}

const SourcePos Position::start() const
{
	return this->range_.start;
}

const SourcePos Position::end() const
{
	return this->range_.end;
}
