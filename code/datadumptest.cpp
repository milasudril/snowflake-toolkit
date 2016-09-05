//@	{"targets":[{"name":"datadumptest","type":"application","cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}}]}

#include "datadump.h"

struct Foo
	{
	int64_t x;
	double y;

	static const SnowflakeModel::DataDump::FieldDescriptor s_fields[];
	};

const SnowflakeModel::DataDump::FieldDescriptor Foo::s_fields[]=
		{
		 {"x",SnowflakeModel::offsetof(&Foo::x),SnowflakeModel::DataDump::Type::INTEGER}
		,{"y",SnowflakeModel::offsetof(&Foo::y),SnowflakeModel::DataDump::Type::DOUBLE}
		};

int main()
	{
	SnowflakeModel::DataDump dump("test.h5");

	Foo obj{1,2.0};
	dump.write("obj",&obj,1);
	return 0;
	}