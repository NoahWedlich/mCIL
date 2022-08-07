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

	bool includes(SourceRange other) const;

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
	Position(const Position& pos);
	Position(const Position& start, const Position& end);

	const SourceRange range() const;
	const SourcePos start_pos() const;
	const SourcePos end_pos() const;
	
	size_t begin();
	size_t end();
private:
	SourceRange range_;
};