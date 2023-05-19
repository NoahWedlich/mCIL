#include "Type.h"

Type Type::make(std::string name, TypeQualifier flags)
{
	TypeID id = type_id(name);
	return *(new Type(id, flags));
}

Type Type::make(TypeID id, TypeQualifier flags)
{
	return *(new Type(id, flags));
}

bool Type::is(TypeID id) const
{
	return id_ == id;
}

bool Type::is(Type other) const
{
	return id_ == other.id_;
}

bool Type::is_subtype_of(TypeID id) const
{
	if (id_ == id) { return true; }
	TypeID super_type = TypeTable::get_super_type(id);
	while (super_type != type_id("type"))
	{
		if (super_type == id)
		{ return true; }
	}
	return false;
}

bool Type::is_subtype_of(Type other) const
{
	if (is(other)) { return true; }
	TypeID super_type = TypeTable::get_super_type(other.id());
	while (super_type != type_id("type"))
	{
		if (super_type == other.id())
		{ return true; }
	}
	return false;
}

const TypeQualifier Type::flags() const
{
	return flags_;
}

const bool Type::is_ptr() const
{
	return (flags_ & TypeFlags::PTR) == TypeFlags::PTR;
}

const bool Type::is_volatile() const
{
	return (flags_ & TypeFlags::VOLATILE) == TypeFlags::VOLATILE;
}

const bool Type::is_nullable() const
{
	return (flags_ & TypeFlags::NULLABLE) == TypeFlags::NULLABLE;
}

const bool Type::is_const() const
{
	return (flags_ & TypeFlags::CONST) == TypeFlags::CONST;
}

const bool Type::is_constexpr() const
{
	return (flags_ & TypeFlags::CONSTEXPR) == TypeFlags::CONSTEXPR;
}

const TypeID Type::id() const
{
	return id_;
}

Type::Type(TypeID id, TypeQualifier flags)
	: id_(id), flags_(flags)
{
}
