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
	,size_t rank,size_t n_elems,const void* data)
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