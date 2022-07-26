#include "Object.h"

Object::Object(ObjType type)
	: type_(type), value_() {}


Object::Object(const Object& other)
	: type_(other.type_), value_()
{
	switch (other.type_)
	{
	case ObjType::BOOL:
		this->value_.bool_value = other.value_.bool_value;
		break;
	case ObjType::NUM:
		this->value_.num_value = other.value_.num_value;
		break;
	case ObjType::STR:
		this->value_.str_value = other.value_.str_value;
	}
}

Object& Object::operator=(const Object& other)
{
	this->type_ = other.type_;
	switch (other.type_)
	{
	case ObjType::BOOL:
		this->value_.bool_value = other.value_.bool_value;
		break;
	case ObjType::NUM:
		this->value_.num_value = other.value_.num_value;
		break;
	case ObjType::STR:
		this->value_.str_value = other.value_.str_value;
	}
	return *this;
}

ObjType Object::type() const
{
	return this->type_;
}

bool Object::is_none() const
{
	return this->type_ == ObjType::NONE;
}

bool Object::is_bool() const
{
	return this->type_ == ObjType::BOOL;
}

bool Object::is_num() const
{
	return this->type_ == ObjType::NUM;
}

bool Object::is_str() const
{
	return this->type_ == ObjType::STR;
}

bool Object::is_unk() const
{
	return this->type_ == ObjType::UNKNOWN;
}

bool Object::is_err() const
{
	return this->type_ == ObjType::ERROR;
}

bool Object::bool_value() const
{
	assert(this->type_ == ObjType::BOOL);
	return this->value_.bool_value;
}

double Object::num_value() const
{
	assert(this->type_ == ObjType::NUM);
	return this->value_.num_value;
}

const std::string& Object::str_value() const
{
	assert(this->type_ == ObjType::STR);
	return *this->value_.str_value;
}

Object Object::create_none_object()
{
	return Object(ObjType::NONE);
}

Object Object::create_bool_object(bool val)
{
	Object obj(ObjType::BOOL);
	obj.value_.bool_value = val;
	return obj;
}

Object Object::create_num_object(double val)
{
	Object obj(ObjType::NUM);
	obj.value_.num_value = val;
	return obj;
}

Object Object::create_str_object(const std::string& val)
{
	Object obj(ObjType::STR);
	obj.value_.str_value = &val;
	return obj;
}

Object Object::create_unkown_object()
{
	return Object(ObjType::UNKNOWN);
}

Object Object::create_error_object()
{
	return Object(ObjType::ERROR);
}

const std::string Object::to_string() const
{
	switch (this->type_)
	{
	case ObjType::NONE:
		return "(None)";
	case ObjType::BOOL:
		return "(Bool: " + this->value_.bool_value ? "true)" : "false)";
	case ObjType::NUM:
		return "(Num: " + std::to_string(this->value_.num_value) + ")";
	case ObjType::STR:
		return "(Str: " + *this->value_.str_value + ")";
	case ObjType::UNKNOWN:
		return "(Unknown)";
	case ObjType::ERROR:
		return "(Error)";
	}
	return "";
}

Object Object::to_bool()
{
	switch (this->type_)
	{
	case ObjType::NONE:
		return Object::create_bool_object(false);
	case ObjType::BOOL:
		return Object::create_bool_object(this->bool_value());
	case ObjType::NUM:
		return Object::create_bool_object(this->num_value() != 0);
	case ObjType::STR:
		return Object::create_bool_object(this->str_value() != "");
	default:
		return Object::create_bool_object(false);
	}
}