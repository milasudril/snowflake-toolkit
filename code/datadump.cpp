//@	{
//@	"targets":
//@		[{
//@		"name":"datadump.o","type":"object"
//@		,"dependencies":
//@			[
//@			 {"ref":"hdf5_hl_cpp","rel":"external"}
//@			,{"ref":"hdf5_cpp","rel":"external"}
//@			,{"ref":"hdf5","rel":"external"}
//@			]
//@		,"cxxoptions":{"cflags_extra":["I/usr/include/hdf5/serial","L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}
//@		}]
//@	}

#include "datadump.h"
#include <H5Cpp.h>
#include <vector>
#include <cassert>

using namespace SnowflakeModel;

H5::StrType DataDump::s_cstr(H5::PredType::C_S1, H5T_VARIABLE);

namespace SnowflakeModel
{
template<>
const H5::DataType& DataDump::MetaObject<bool>::typeGet() const noexcept
	{return H5::PredType::NATIVE_CHAR;}

template<>
const H5::DataType& DataDump::MetaObject<char>::typeGet() const noexcept
	{return H5::PredType::NATIVE_CHAR;}

template<>
const H5::DataType& DataDump::MetaObject<short>::typeGet() const noexcept
	{return H5::PredType::NATIVE_SHORT;}

template<>
const H5::DataType& DataDump::MetaObject<int>::typeGet() const noexcept
	{return H5::PredType::NATIVE_INT;}

template<>
const H5::DataType& DataDump::MetaObject<long>::typeGet() const noexcept
	{return H5::PredType::NATIVE_LONG;}

template<>
const H5::DataType& DataDump::MetaObject<long long>::typeGet() const noexcept
	{return H5::PredType::NATIVE_LLONG;}

template<>
const H5::DataType& DataDump::MetaObject<float>::typeGet() const noexcept
	{return H5::PredType::NATIVE_FLOAT;}

template<>
const H5::DataType& DataDump::MetaObject<double>::typeGet() const noexcept
	{return H5::PredType::NATIVE_DOUBLE;}



template<>
const H5::DataType& DataDump::MetaObject<unsigned char>::typeGet() const noexcept
	{return H5::PredType::NATIVE_UCHAR;}

template<>
const H5::DataType& DataDump::MetaObject<unsigned short>::typeGet() const noexcept
	{return H5::PredType::NATIVE_USHORT;}

template<>
const H5::DataType& DataDump::MetaObject<unsigned int>::typeGet() const noexcept
	{return H5::PredType::NATIVE_UINT;}

template<>
const H5::DataType& DataDump::MetaObject<unsigned long>::typeGet() const noexcept
	{return H5::PredType::NATIVE_ULONG;}

template<>
const H5::DataType& DataDump::MetaObject<unsigned long long>::typeGet() const noexcept
	{return H5::PredType::NATIVE_ULLONG;}

template<>
DataDump::MetaObject<const char*>::MetaObject():r_cstr(s_cstr){}
}




DataDump::DataTypeHandle
	DataDump::compoundMake(const FieldDescriptor* fields
	,size_t n_fields,size_t struct_size)
	{
	auto obj=new H5::CompType(struct_size);

	auto fields_end=fields+n_fields;
	while(fields!=fields_end)
		{
		assert(&fields->type!=nullptr);
		obj->insertMember(fields->name,fields->offset,fields->type);
		++fields;
		}

	return DataTypeHandle(obj,deleter);
	}

void DataDump::dataWrite(const H5::DataType& type,const char* objname
	,size_t n_elems,const void* data)
	{
	hsize_t dim[]={n_elems};
	H5::DataSpace space(1,dim);
	H5::DataSet ds(m_file->createDataSet(objname,type,space));
	ds.write(data,type);
	}

void DataDump::dataWrite(const H5::DataType& type,const char* objname
	,size_t n_rows,size_t n_cols,const void* data)
	{
	hsize_t dim[]={n_rows,n_cols};
	H5::DataSpace space(2,dim);
	H5::DataSet ds(m_file->createDataSet(objname,type,space));
	ds.write(data,type);
	}

DataDump::GroupHandle DataDump::groupCreate(const char* name)
	{
	return GroupHandle(new H5::Group(m_file->createGroup(name)), deleter);
	}

DataDump::DataDump(const char* filename):
	m_file(new H5::H5File(filename, H5F_ACC_TRUNC))
	{}

DataDump::~DataDump()
	{}

void DataDump::deleter(H5::DataType* obj)
	{delete obj;}

void DataDump::deleter(H5::Group* obj)
	{delete obj;}

template<>
void DataDump::write<std::string>(const char* objname,const std::string* data,size_t n_elems)
	{
	std::vector<const char*> strptr;
	auto pos=data;
	auto pos_end=pos+n_elems;
	while(pos!=pos_end)
		{
		strptr.push_back(pos->c_str());
		++pos;
		}

	dataWrite(s_cstr,objname,strptr.size(),strptr.data());
	}

