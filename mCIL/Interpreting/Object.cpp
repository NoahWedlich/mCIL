#include "Object.h"

Object::Object(cilType type)
	: type_(type), value_() {}


Object::Object(const Object& other)
	: type_(other.type_), value_()
{
	switch (other.type_.type)
	{
	case Type::BOOL:
		this->value_.bool_value = other.value_.bool_value;
		break;
	case Type::NUM:
		this->value_.num_value = other.value_.num_value;
		break;
	case Type::STR:
		this->value_.str_value = other.value_.str_value;
	}
}

Object& Object::operator=(const Object& other)
{
	this->type_ = other.type_;
	switch (other.type_.type)
	{
	case Type::BOOL:
		this->value_.bool_value = other.value_.bool_value;
		break;
	case Type::NUM:
		this->value_.num_value = other.value_.num_value;
		break;
	case Type::STR:
		this->value_.str_value = other.value_.str_value;
	}
	return *this;
}

cilType Object::type() const
{
	return this->type_;
}

bool Object::is_none() const
{
	return this->type_ == Type::NONE;
}

bool Object::is_bool() const
{
	return this->type_ == Type::BOOL;
}

bool Object::is_num() const
{
	return this->type_ == Type::NUM;
}

bool Object::is_str() const
{
	return this->type_ == Type::STR;
}

bool Object::is_unk() const
{
	return this->type_ == Type::UNKNOWN;
}

bool Object::is_err() const
{
	return this->type_ == Type::ERROR;
}

bool Object::bool_value() const
{
	assert(this->type_ == Type::BOOL);
	return this->value_.bool_value;
}

double Object::num_value() const
{
	assert(this->type_ == Type::NUM);
	return this->value_.num_value;
}

const std::string& Object::str_value() const
{
	assert(this->type_ == Type::STR);
	return *this->value_.str_value;
}

Object Object::create_none_object()
{
	return Object(Type::NONE);
}

Object Object::create_bool_object(bool val)
{
	Object obj(Type::BOOL);
	obj.value_.bool_value = val;
	return obj;
}

Object Object::create_num_object(double val)
{
	Object obj(Type::NUM);
	obj.value_.num_value = val;
	return obj;
}

Object Object::create_str_object(const std::string& val)
{
	Object obj(Type::STR);
	obj.value_.str_value = &val;
	return obj;
}

Object Object::create_unkown_object()
{
	return Object(Type::UNKNOWN);
}

Object Object::create_error_object()
{
	return Object(Type::ERROR);
}

const std::string Object::to_string() const
{
	switch (this->type_.type)
	{
	case Type::NONE:
		return "(None)";
	case Type::BOOL:
		return this->value_.bool_value ? "(Bool: true)" : "(Bool: false)";
	case Type::NUM:
		//TODO: Remove trailing zeros and dot
		return "(Num: " + std::to_string(this->value_.num_value) + ")";
	case Type::STR:
		return "(Str: " + *this->value_.str_value + ")";
	case Type::UNKNOWN:
		return "(Unknown)";
	case Type::ERROR:
		return "(Error)";
	}
	return "";
}

Object Object::to_bool()
{
	switch (this->type_.type)
	{
	case Type::NONE:
		return Object::create_bool_object(false);
	case Type::BOOL:
		return Object::create_bool_object(this->bool_value());
	case Type::NUM:
		return Object::create_bool_object(this->num_value() != 0);
	case Type::STR:
		return Object::create_bool_object(this->str_value() != "");
	default:
		return Object::create_bool_object(false);
	}
}