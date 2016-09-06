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

using namespace SnowflakeModel;

static const H5::PredType& mapType(DataDump::Type type)
	{
	switch(type)
		{
		case DataDump::Type::INTEGER:
			return H5::PredType::NATIVE_INT64;
		case DataDump::Type::DOUBLE:
			return H5::PredType::NATIVE_DOUBLE;
	/*	FIXME: HDF5 cannot handle std::string so this needs to be handled differently
		case DataDump::Type::STRING:
			return H5::PredType::C_S1;*/
		}
//	This should never happen
	return H5::PredType::NATIVE_INT64;
	}

std::unique_ptr<H5::CompType,void(*)(H5::CompType*)>
	DataDump::compoundMake(const FieldDescriptor* fields
	,size_t n_fields,size_t struct_size)
	{
	std::unique_ptr<H5::CompType,void(*)(H5::CompType*)> ret
		(new H5::CompType(struct_size),deleter);

	auto fields_end=fields+n_fields;
	while(fields!=fields_end)
		{
		ret->insertMember(fields->name,fields->offset,mapType(fields->type));
		++fields;
		}

	return std::move(ret);
	}

void DataDump::dataWrite(const H5::CompType& type,const char* objname
	,size_t n_elems,const void* data)
	{
	dataWrite(static_cast<const H5::DataType&>(type),objname,n_elems,data);
	}

void DataDump::dataWrite(const H5::DataType& type,const char* objname
	,size_t n_elems,const void* data)
	{
	hsize_t dim[]={n_elems};
	H5::DataSpace space(1,dim);
	H5::DataSet ds(m_file->createDataSet(objname,type,space));
	ds.write(data,type);
	}

DataDump::DataDump(const char* filename):
	m_file(new H5::H5File(filename, H5F_ACC_TRUNC))
	{}

DataDump::~DataDump()
	{}

void DataDump::deleter(H5::CompType* obj)
	{delete obj;}


template<>
void DataDump::write<char>(const char* objname,const char* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_CHAR,objname,n_elems,data);
	}

template<>
void DataDump::write<short>(const char* objname,const short* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_SHORT,objname,n_elems,data);
	}

template<>
void DataDump::write<int>(const char* objname,const int* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_INT,objname,n_elems,data);
	}

template<>
void DataDump::write<long>(const char* objname,const long* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_LONG,objname,n_elems,data);
	}

template<>
void DataDump::write<long long>(const char* objname,const long long* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_LLONG,objname,n_elems,data);
	}




template<>
void DataDump::write<unsigned char>(const char* objname
	,const unsigned char* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_UCHAR,objname,n_elems,data);
	}

template<>
void DataDump::write<unsigned short>(const char* objname,const unsigned short* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_USHORT,objname,n_elems,data);
	}

template<>
void DataDump::write<unsigned int>(const char* objname,const unsigned int* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_UINT,objname,n_elems,data);
	}

template<>
void DataDump::write<unsigned long>(const char* objname,const unsigned long* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_ULONG,objname,n_elems,data);
	}

template<>
void DataDump::write<unsigned long long>(const char* objname,const unsigned long long* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_ULLONG,objname,n_elems,data);
	}


template<>
void DataDump::write<float>(const char* objname,const float* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_FLOAT,objname,n_elems,data);
	}

template<>
void DataDump::write<double>(const char* objname,const double* data,size_t n_elems)
	{
	dataWrite(H5::PredType::NATIVE_DOUBLE,objname,n_elems,data);
	}

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

	H5::StrType datatype(H5::PredType::C_S1, H5T_VARIABLE);
	dataWrite(datatype,objname,strptr.size(),strptr.data());
	}
