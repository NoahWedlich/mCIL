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

bool SourceRange::includes(SourceRange other) const
{
	return start <= other.start && end >= other.start;
}

Position::Position(size_t line_off, size_t char_off)
{
	SourcePos pos{ line_off, char_off };
	range_ = SourceRange{ pos, pos };
}

Position::Position(const SourcePos& pos)
	: range_(pos, pos) {}

Position::Position(const SourcePos& start, const SourcePos& end)
	: range_(start, end) {}

Position::Position(const SourceRange& range)
	: range_(range) {}

Position::Position(const Position& pos)
	: range_(pos.range()) {}

Position::Position(const Position& start, const Position& end)
	: range_(start.start_pos(), end.end_pos()) {}

const SourceRange Position::range() const
{
	return this->range_;
}

const SourcePos Position::start_pos() const
{
	return this->range_.start;
}

const SourcePos Position::end_pos() const
{
	return this->range_.end;
}

size_t Position::begin()
{ return this->range_.start.char_off; }

size_t Position::end()
{ return this->range_.end.char_off + 1; }
