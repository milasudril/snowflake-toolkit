//@	{"targets":[{"name":"datadumptest","type":"application","cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}}]}

#include "datadump.h"

struct Foo
	{
	int64_t x;
	double y;
	};

namespace SnowflakeModel
	{
	template<>
	struct MetaObject<Foo>
		{
		static constexpr const DataDump::FieldDescriptor fields[]=
			{
			 {"x",SnowflakeModel::offsetof(&Foo::x),SnowflakeModel::DataDump::Type::INTEGER}
			,{"y",SnowflakeModel::offsetof(&Foo::y),SnowflakeModel::DataDump::Type::DOUBLE}
			};

		};
	constexpr const DataDump::FieldDescriptor MetaObject<Foo>::fields[];
	}

int main()
	{
	SnowflakeModel::DataDump dump("test.h5");

	Foo obj{1,2.0};
	dump.write("obj",&obj,1);

	std::string str("Flygande bäckasiner söka hwila på mjuka tufvor");
	dump.write("str",str.data(),str.size());
	dump.write("str2",&str,1);

	long x[4]={1,2,3,4};
	dump.write("x",x,4);

	return 0;
	}