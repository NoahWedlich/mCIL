#pragma once
#include <stdexcept>

struct SourcePos
{
	SourcePos()
		: line_off(0), char_off(0) {}

	SourcePos(size_t line_off, size_t char_off)
		: line_off(line_off), char_off(char_off) {}

	bool operator<(const SourcePos& other) const;
	bool operator>(const SourcePos& other) const;
	bool operator<=(const SourcePos& other) const;
	bool operator>=(const SourcePos& other) const;

	size_t line_off;
	size_t char_off;
};

struct SourceRange
{
	SourceRange()
		: start(), end(), multi_line(false) {}

	SourceRange(SourcePos start, SourcePos end);

	bool includes(SourceRange other);

	SourcePos start;
	SourcePos end;
	bool multi_line;
};

class Position
{
public:
	Position(size_t line_off, size_t char_off);
	Position(const SourcePos& pos);
	Position(const SourcePos& start, const SourcePos& end);
	Position(const SourceRange& range);
	Position(const SourceRange& range, const SourceRange& focus);

	const SourceRange range() const;
	const SourceRange focus() const;
	const SourcePos start() const;
	const SourcePos end() const;
private:
	SourceRange range_;
	SourceRange focus_;
};