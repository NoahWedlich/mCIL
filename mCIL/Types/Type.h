#pragma once
#include "TypeTable.h"

typedef unsigned char TypeQualifier;

const enum TypeFlags
{
	PTR       = 1,
	VOLATILE  = 2,
	NULLABLE  = 4,
	CONST     = 8,
	CONSTEXPR = 16
};

class Type
{
public:
	static Type make(std::string name, TypeQualifier flags = 0);
	static Type make(TypeID id, TypeQualifier flags = 0);

	bool is(TypeID id) const;
	bool is(Type other) const;

	bool is_subtype_of(TypeID id) const;
	bool is_subtype_of(Type other) const;

	const TypeQualifier flags() const;
	const bool is_ptr() const;
	const bool is_volatile() const;
	const bool is_nullable() const;
	const bool is_const() const;
	const bool is_constexpr() const;

	const TypeID id() const;
private:
	Type(TypeID id, TypeQualifier flags);

	TypeID id_;
	TypeQualifier flags_;
};