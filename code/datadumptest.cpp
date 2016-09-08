//@	{"targets":[{"name":"datadumptest","type":"application","cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}}]}

#include "datadump.h"

struct Bar
	{
	float a;
	float b;
	};

struct Foo
	{
	const char* key;
	int value;
	};

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<Bar>::fields[]=
		{
		 {"a",SnowflakeModel::offsetOf(&Bar::a),SnowflakeModel::DataDump::MetaObject<decltype(Bar::a)>().typeGet()}
		,{"b",SnowflakeModel::offsetOf(&Bar::b),SnowflakeModel::DataDump::MetaObject<decltype(Bar::b)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<Bar>::field_count=2;

	static const DataDump::MetaObject<Bar> bar;

	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<Foo>::fields[]=
		{
		 {"key",SnowflakeModel::offsetOf(&Foo::key),SnowflakeModel::DataDump::MetaObject<decltype(Foo::key)>().typeGet()}
		,{"value",SnowflakeModel::offsetOf(&Foo::value),SnowflakeModel::DataDump::MetaObject<decltype(Foo::value)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<Foo>::field_count=2;
	}

int main()
	{
	SnowflakeModel::DataDump dump("test.h5");

	Foo obj{"Hello, World",1};
	dump.write("obj",&obj,1);

	std::string str("Flygande bäckasiner söka hwila på mjuka tufvor");
	dump.write("str",str.data(),str.size());
	dump.write("str2",&str,1);

	long x[4]={1,2,3,4};
	dump.write("x",x,4);

	long y[2][3]={{1,2,3},{4,5,6}};
	dump.write("y",&y[0][0],2,3);

	return 0;
	}