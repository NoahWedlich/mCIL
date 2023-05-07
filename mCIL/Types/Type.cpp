#include "Type.h"

Type* Type::make(std::string name, TypeQualifier flags)
{
	if(!TypeTable::type_exists(name))
	{ return nullptr; }
	TypeID id = type_id(name);
	return new Type(id, flags);
}

Type* Type::make(TypeID id, TypeQualifier flags)
{
	if (!TypeTable::type_exists(id))
	{ return nullptr; }
	return new Type(id, flags);
}

bool Type::is(TypeID id)
{
	return id_ == id;
}

bool Type::is(Type* other)
{
	return id_ == other->id_;
}

bool Type::is_subtype_of(TypeID id)
{
	TypeID super_type = TypeTable::get_super_type(id);
	while (super_type != type_id("type"))
	{
		if (super_type == id)
		{ return true; }
	}
	return false;
}

bool Type::is_subtype_of(Type* other)
{
	TypeID super_type = TypeTable::get_super_type(other->id());
	while (super_type != type_id("type"))
	{
		if (super_type == other->id())
		{ return true; }
	}
	return false;
}

const TypeQualifier Type::flags() const
{
	return flags_;
}

const bool Type::is_ptr()
{
	return (flags_ & TypeFlags::PTR) == TypeFlags::PTR;
}

const bool Type::is_volatile()
{
	return (flags_ & TypeFlags::VOLATILE) == TypeFlags::VOLATILE;
}

const bool Type::is_nullable()
{
	return (flags_ & TypeFlags::NULLABLE) == TypeFlags::NULLABLE;
}

const bool Type::is_const()
{
	return (flags_ & TypeFlags::CONST) == TypeFlags::CONST;
}

const bool Type::is_constexpr()
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
