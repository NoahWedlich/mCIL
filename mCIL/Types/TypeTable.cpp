#include "TypeTable.h"

TypeID TypeTable::type_count_ = 0;
std::vector<TypeTable::TypeTableEntry> TypeTable::table_{};

TypeID TypeTable::register_type(std::string name, TypeID super_type)
{
	TypeID id = type_count_++;
	TypeTableEntry entry{ name, id, super_type };
	table_.push_back(entry);
	return id;
}

TypeID TypeTable::get_type_id(std::string name)
{
	for (TypeTableEntry& entry : table_)
	{
		if (entry.name == name)
		{ return entry.id; }
	}
	return 0;
}

std::string TypeTable::get_type_name(TypeID id)
{
	for (TypeTableEntry& entry : table_)
	{
		if (entry.id == id)
		{ return entry.name; }
	}
	return "error";
}

TypeID TypeTable::get_super_type(TypeID id)
{
	for (TypeTableEntry& entry : table_)
	{
		if (entry.id == id)
		{ return entry.parent; }
	}
	return 0;
}

bool TypeTable::type_exists(TypeID id)
{
	for (TypeTableEntry& entry : table_)
	{
		if (entry.id == id)
		{ return true; }
	}
	return false;
}

bool TypeTable::type_exists(std::string name)
{
	for (TypeTableEntry& entry : table_)
	{
		if (entry.name == name)
		{ return true; }
	}
	return false;
}

void TypeTable::add_builtin_types()
{
	TypeTable::register_type("type"     , type_id("type"    )); 
	TypeTable::register_type("error"    , type_id("type"    ));
	TypeTable::register_type("value"    , type_id("type"    ));    
	TypeTable::register_type("object"   , type_id("type"    ));    
	TypeTable::register_type("iterable" , type_id("type"    ));    
	TypeTable::register_type("enum"     , type_id("type"    ));    
	TypeTable::register_type("num"      , type_id("value"   ));   
	TypeTable::register_type("int"      , type_id("value"   ));   
	TypeTable::register_type("bool"     , type_id("value"   ));   
	TypeTable::register_type("function" , type_id("object"  ));  
	TypeTable::register_type("struct"   , type_id("object"  ));  
	TypeTable::register_type("union"    , type_id("object"  ));  
	TypeTable::register_type("str"      , type_id("iterable"));
	TypeTable::register_type("strview"  , type_id("iterable"));
	TypeTable::register_type("array"    , type_id("iterable"));
	TypeTable::register_type("list"     , type_id("iterable"));
	TypeTable::register_type("map"      , type_id("iterable"));
	TypeTable::register_type("float"    , type_id("num"     ));     
	TypeTable::register_type("double"   , type_id("num"     ));     
	TypeTable::register_type("apfloat"  , type_id("num"     ));     
	TypeTable::register_type("complex"  , type_id("num"     ));     
	TypeTable::register_type("uint"     , type_id("int"     ));     
	TypeTable::register_type("sint"     , type_id("int"     ));     
	TypeTable::register_type("char"     , type_id("int"     ));     
	TypeTable::register_type("uint2"    , type_id("uint"    ));    
	TypeTable::register_type("uint4"    , type_id("uint"    ));    
	TypeTable::register_type("uint8"    , type_id("uint"    ));    
	TypeTable::register_type("uint16"   , type_id("uint"    ));    
	TypeTable::register_type("uint32"   , type_id("uint"    ));    
	TypeTable::register_type("uint64"   , type_id("uint"    ));    
	TypeTable::register_type("uint128"  , type_id("uint"    ));    
	TypeTable::register_type("sint2"    , type_id("sint"    ));    
	TypeTable::register_type("sint4"    , type_id("sint"    ));    
	TypeTable::register_type("sint8"    , type_id("sint"    ));    
	TypeTable::register_type("sint16"   , type_id("sint"    ));    
	TypeTable::register_type("sint32"   , type_id("sint"    ));    
	TypeTable::register_type("sint64"   , type_id("sint"    ));    
	TypeTable::register_type("sint128"  , type_id("sint"    ));    
	TypeTable::register_type("asciichar", type_id("char"    ));    
	TypeTable::register_type("utf8char" , type_id("char"    ));    
	TypeTable::register_type("utf16char", type_id("char"    ));    
}
