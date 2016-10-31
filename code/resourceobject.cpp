//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[
//@	                {
//@	                    "ref":"jansson",
//@	                    "rel":"external"
//@	                }
//@	            ],
//@	            "name":"resourceobject.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}

#include "resourceobject.h"
#include "datasource.h"

#include <jansson.h>

using namespace SnowflakeModel;

static size_t loadCallback(void* buffer, size_t length, void* eventhandler)
	{
	return reinterpret_cast<DataSource*>(eventhandler)->read(buffer,length);
	}

ResourceObject::Iterator::Iterator(ResourceObject& object):r_object(object)
	{
	m_handle=json_object_iter(static_cast<json_t*>(object.m_handle));
	}

std::pair<const char*,ResourceObject> ResourceObject::Iterator::get() noexcept
	{
	auto key=json_object_iter_key(m_handle);
	ResourceObject value{json_object_iter_value(m_handle),key};

	return {key,std::move(value)};
	}

void ResourceObject::Iterator::next() noexcept
	{
	m_handle=json_object_iter_next(static_cast<json_t*>(r_object.m_handle),m_handle);
	}

bool ResourceObject::Iterator::endAt() noexcept
	{
	return m_handle==nullptr;
	}



ResourceObject::ResourceObject(DataSource& readhandler)
	{
	json_error_t status;
	m_handle=json_load_callback(loadCallback,&readhandler,0,&status);
	if(m_handle==nullptr)
		{
		throw "Could not load JSON data";
		}
	}

ResourceObject::ResourceObject(void* handle,const char* name)
	{
	m_handle=handle;
	json_incref(static_cast<json_t*>(m_handle));
	}

ResourceObject::~ResourceObject()
	{
	json_decref(static_cast<json_t*>(m_handle));
	}

ResourceObject::Type ResourceObject::typeGet() const noexcept
	{
	switch(json_typeof(static_cast<const json_t*>(m_handle)))
		{
		case JSON_OBJECT:
			return Type::OBJECT;

		case JSON_ARRAY:
			return Type::ARRAY;

		case JSON_STRING:
			return Type::STRING;

		case JSON_INTEGER:
			return Type::INTEGER;

		case JSON_REAL:
			return Type::FLOAT;

		default:
			return Type::OBJECT;
		}
	}

ResourceObject ResourceObject::objectGet(const char* name) const
	{
	auto result=json_object_get(static_cast<const json_t*>(m_handle),name);
	if(result==NULL)
		{
		throw "Could not get child object";
		}

	return ResourceObject(result,name);
	}

bool ResourceObject::objectExists(const char* name) const noexcept
	{
	return json_object_get(static_cast<const json_t*>(m_handle),name)!=NULL;
	}

size_t ResourceObject::objectCountGet() const noexcept
	{return json_array_size(static_cast<const json_t*>(m_handle));}

ResourceObject ResourceObject::objectGet(size_t index) const
	{
	auto result=json_array_get(static_cast<const json_t*>(m_handle),index);
	if(result==NULL)
		{
		throw "Could not get child object by number";
		}
	return ResourceObject(result,nullptr);
	}

const char* ResourceObject::stringGet(const char* name) const noexcept
	{
	auto string_obj=json_object_get(static_cast<const json_t*>(m_handle),name);
	if(string_obj!=NULL)
		{
		auto string=json_string_value(static_cast<const json_t*>(string_obj));
		if(string!=NULL)
			{return string;}
		}
	return nullptr;
	}

const char* ResourceObject::stringGet() const noexcept
	{
	auto ret=json_string_value(static_cast<const json_t*>(m_handle));
	if(ret==NULL)
		{return nullptr;}
	return ret;
	}

long long int ResourceObject::integerGet() const noexcept
	{return json_integer_value(static_cast<const json_t*>(m_handle));}

double ResourceObject::floatGet() const noexcept
	{
	if(typeGet()==Type::INTEGER)
		{
		return static_cast<double>(integerGet());
		}
	return json_real_value(static_cast<const json_t*>(m_handle));
	}

ResourceObject::operator const char*() const
	{
	auto ret=stringGet();
	if(ret==nullptr)
		{
		throw "Current resource object is not a string";
		}
	return ret;
	}

ResourceObject::operator long long int() const
	{
	if(typeGet()!=Type::INTEGER)
		{
		throw "Current resource object is not an integer";
		}
	return integerGet();
	}

ResourceObject::operator double() const
	{
	switch(typeGet())
		{
		case Type::INTEGER:
		case Type::FLOAT:
			return floatGet();

		default:
			throw "Current resource object is not an integer";
		}
	}
