//@	{"targets":[{"name":"datadumptest","type":"application","cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}}]}

#include "datadump.h"

struct Bar
	{
	float a;
	float b;
	};

struct Foo
	{
	Bar obj;
	int64_t x;
	double y;
	const char* str;
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
		 {"obj",SnowflakeModel::offsetOf(&Foo::obj),bar.typeGet()}
		,{"x",SnowflakeModel::offsetOf(&Foo::x),SnowflakeModel::DataDump::MetaObject<decltype(Foo::x)>().typeGet()}
		,{"y",SnowflakeModel::offsetOf(&Foo::y),SnowflakeModel::DataDump::MetaObject<decltype(Foo::y)>().typeGet()}
		,{"str",SnowflakeModel::offsetOf(&Foo::str),SnowflakeModel::DataDump::MetaObject<decltype(Foo::str)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<Foo>::field_count=4;
	}

int main()
	{
	SnowflakeModel::DataDump dump("test.h5");

	Foo obj{{2.2f,245.0f},1,2.0,"Hello, World"};
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