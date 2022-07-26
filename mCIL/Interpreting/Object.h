#pragma once
#include "../cil-system.h"

enum class ObjType
{
	NONE,
	BOOL,
	NUM,
	STR,
	UNKNOWN,
	ERROR
};

class Object
{
public:
	Object(const Object&);
	Object& operator=(const Object&);

	ObjType type() const;

	bool is_none() const;
	bool is_bool() const;
	bool is_num() const;
	bool is_str() const;
	bool is_unk() const;
	bool is_err() const;

	bool bool_value() const;
	double num_value() const;
	const std::string& str_value() const;

	static Object create_none_object();
	static Object create_bool_object(bool val);
	static Object create_num_object(double val);
	static Object create_str_object(const std::string& val);
	static Object create_unkown_object();
	static Object create_error_object();
	
	const std::string to_string() const;
	Object to_bool();

private:
	Object(ObjType type);

	ObjType type_;

	union {
		bool bool_value;
		double num_value;
		const std::string* str_value;
	} value_;
};

