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

template<>
DataDump::MetaObject<char*>::MetaObject():r_cstr(s_cstr){}
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


struct DataDump::ArrayReaderImpl
	{
	ArrayReaderImpl(const H5::H5File& src,const H5::DataType& type
		,const char* objname):ds(src.openDataSet(objname))
		,space(ds.getSpace()),r_type(type),offset(0)
		{
		auto rank=space.getSimpleExtentNdims();
		if(rank!=1)
			{throw "Rank must be equal to 1";}
		space.getSimpleExtentDims(&size);
		}

	H5::DataSet ds;
	H5::DataSpace space;
	H5::DataType r_type;
	hsize_t offset;
	hsize_t size;
	};

void DataDump::deleter(ArrayReaderImpl* obj)
	{delete obj;}

size_t DataDump::dataRead(ArrayReaderImpl& reader,void* buffer,size_t n_elems)
	{
	hsize_t n=n_elems;
	hsize_t count=std::min(n,reader.size-reader.offset);
	reader.space.selectHyperslab(H5S_SELECT_SET, &count, &reader.offset);
	reader.offset+=count;

	H5::DataSpace mem(1,&n);
	hsize_t zero=0;
	mem.selectHyperslab(H5S_SELECT_SET, &count, &zero);
	reader.ds.read(buffer,reader.r_type,mem,reader.space);
	return count;
	}

uintmax_t DataDump::size(const ArrayReaderImpl& impl)
	{
	return impl.size;
	}

DataDump::ArrayReaderHandle DataDump::arrayReaderCreate(const H5::DataType& type,const char* objname) const
	{
	return {new ArrayReaderImpl(*m_file,type,objname),deleter};
	}

void DataDump::dataRead(const H5::DataType& type,const char* objname
	,size_t n_elems,void* data) const
	{
	auto ds=m_file->openDataSet(objname);
	auto space=ds.getSpace();
	auto rank=space.getSimpleExtentNdims();
	if(rank!=1)
		{throw "Rank must be 1";}
	hsize_t dim;
	space.getSimpleExtentDims(&dim);

	hsize_t n=n_elems;
	hsize_t offset=0;   // hyperslab offset in the file
	hsize_t count=std::min(n,dim);
	space.selectHyperslab(H5S_SELECT_SET, &count, &offset);

	H5::DataSpace mem(1,&n);
	mem.selectHyperslab(H5S_SELECT_SET, &count, &offset);

	ds.read(data,type,mem,space);
	}



DataDump::GroupHandle DataDump::groupCreate(const char* name)
	{
	return GroupHandle(new H5::Group(m_file->createGroup(name)), deleter);
	}

static int getMode(DataDump::IOMode mode)
	{
	switch(mode)
		{
		case DataDump::IOMode::WRITE:
			return H5F_ACC_TRUNC;
		case DataDump::IOMode::READ:
			return H5F_ACC_RDONLY;
		}
	return H5F_ACC_RDONLY;
	}

DataDump::DataDump(const char* filename,IOMode mode):
	m_file(new H5::H5File(filename, getMode(mode)))
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

