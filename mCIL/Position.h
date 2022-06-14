#pragma once
struct Position
{
	Position(size_t line, size_t start, size_t length = 1)
		: line(line), startChar(start), length(length) {}

	size_t line;
	size_t startChar;
	size_t length;
};

