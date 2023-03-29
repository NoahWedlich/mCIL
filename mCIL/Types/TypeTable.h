#pragma once
#include "../cil-system.h"

typedef unsigned char TypeID;

class TypeTable
{
public:
	static TypeID register_type(std::string name, TypeID super_type);

	static TypeID get_type_id(std::string name);
	static std::string get_type_name(TypeID id);
	static TypeID get_super_type(TypeID id);

	static bool type_exists(TypeID id);
	static bool type_exists(std::string name);

	static void add_builtin_types();
private:
	struct TypeTableEntry
	{
		std::string name;
		TypeID id;
		TypeID parent;
	};

	static TypeID type_count_;

	static std::vector<TypeTableEntry> table_;
};

#define type_id(name) TypeTable::get_type_id(name)
#define type_name(id) TypeTable::get_type_name(id)
