#include "Object.h"

Object::Object(obj_type type)
	: type_(type), value_() {}


Object::Object(const Object& other)
	: type_(other.type_), value_()
{
	switch (other.type_)
	{
	case obj_type::BOOL:
		this->value_.bool_value = other.value_.bool_value;
		break;
	case obj_type::NUM:
		this->value_.num_value = other.value_.num_value;
		break;
	case obj_type::STR:
		this->value_.str_value = other.value_.str_value;
	}
}

Object& Object::operator=(const Object& other)
{
	this->type_ = other.type_;
	switch (other.type_)
	{
	case obj_type::BOOL:
		this->value_.bool_value = other.value_.bool_value;
		break;
	case obj_type::NUM:
		this->value_.num_value = other.value_.num_value;
		break;
	case obj_type::STR:
		this->value_.str_value = other.value_.str_value;
	}
	return *this;
}

obj_type Object::type() const
{
	return this->type_;
}

bool Object::is_none() const
{
	return this->type_ == obj_type::NONE;
}

bool Object::is_bool() const
{
	return this->type_ == obj_type::BOOL;
}

bool Object::is_num() const
{
	return this->type_ == obj_type::NUM;
}

bool Object::is_str() const
{
	return this->type_ == obj_type::STR;
}

bool Object::is_unk() const
{
	return this->type_ == obj_type::UNKNOWN;
}

bool Object::is_err() const
{
	return this->type_ == obj_type::ERROR;
}

bool Object::bool_value() const
{
	assert(this->type_ == obj_type::BOOL);
	return this->value_.bool_value;
}

double Object::num_value() const
{
	assert(this->type_ == obj_type::NUM);
	return this->value_.num_value;
}

const std::string& Object::str_value() const
{
	assert(this->type_ == obj_type::STR);
	return *this->value_.str_value;
}

Object Object::create_none_object()
{
	return Object(obj_type::NONE);
}

Object Object::create_bool_object(bool val)
{
	Object obj(obj_type::BOOL);
	obj.value_.bool_value = val;
	return obj;
}

Object Object::create_num_object(double val)
{
	Object obj(obj_type::NUM);
	obj.value_.num_value = val;
	return obj;
}

Object Object::create_str_object(std::string& val)
{
	Object obj(obj_type::STR);
	obj.value_.str_value = &val;
	return obj;
}

Object Object::create_unkown_object()
{
	return Object(obj_type::UNKNOWN);
}

Object Object::create_error_object()
{
	return Object(obj_type::ERROR);
}

const std::string Object::to_string() const
{
	switch (this->type_)
	{
	case obj_type::NONE:
		return "(None)";
	case obj_type::BOOL:
		return "(Bool: " + this->value_.bool_value ? "true)" : "false)";
	case obj_type::NUM:
		return "(Num: " + std::to_string(this->value_.num_value) + ")";
	case obj_type::STR:
		return "(Str: " + *this->value_.str_value + ")";
	case obj_type::UNKNOWN:
		return "(Unknown)";
	case obj_type::ERROR:
		return "(Error)";
	}
	return "";
}