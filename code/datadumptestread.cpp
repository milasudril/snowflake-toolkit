//@	{"targets":[{"name":"datadumptestread","type":"application","cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}}]}

#include "datadump.h"

struct Foo
	{
	SnowflakeModel::DataDump::StringHolder key;
	int value;
	};

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<Foo>::fields[]=
		{
		 {"key",SnowflakeModel::offsetOf(&Foo::key),SnowflakeModel::DataDump::MetaObject<decltype(Foo::key)>().typeGet()}
		,{"value",SnowflakeModel::offsetOf(&Foo::value),SnowflakeModel::DataDump::MetaObject<decltype(Foo::value)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<Foo>::field_count=2;
	}



struct Y
	{
	int a;
	float b;
	};

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<Y>::fields[]=
		{
		 {"a",SnowflakeModel::offsetOf(&Y::a),SnowflakeModel::DataDump::MetaObject<decltype(Y::a)>().typeGet()}
		,{"b",SnowflakeModel::offsetOf(&Y::b),SnowflakeModel::DataDump::MetaObject<decltype(Y::b)>().typeGet()}
		};

	template<>
	const size_t DataDump::MetaObject<Y>::field_count=2;
	}

int main()
	{
	SnowflakeModel::DataDump dump("test.h5",SnowflakeModel::DataDump::IOMode::READ);

/*		{
		int x[3]={0,0,0};
		auto reader=dump.arrayGet<int>("test_array");
		size_t n;
		do
			{
			n=reader.dataRead(x,3);
			printf("Got %zu values:",n);
			for(size_t k=0;k<n;++k)
				{printf(" %d",x[k]);}
			putchar('\n');
			}
		while(n==3);
		}

		{
		auto reader=dump.arrayGet<int>("test_array");
		auto data=reader.dataRead();
		auto n=data.size();
		printf("Got %zu values:",n);
		for(size_t k=0;k<n;++k)
			{printf("    %d",data[k]);}
		putchar('\n');
		}

		{
		auto reader=dump.arrayGet<Y>("simple");
		auto data=reader.dataRead();
		auto n=data.size();
		printf("Got %zu values:",n);
		for(size_t k=0;k<n;++k)
			{printf("%d %.7g\n",data[k].a,data[k].b);}
		putchar('\n');
		}*/


		{
		auto reader=dump.arrayGet<Foo>("obj");
		auto data=reader.dataRead();
		auto n=data.size();
		printf("Got %zu values:\n",n);
		for(size_t k=0;k<n;++k)
			{printf("    %s %d",data[k].key.begin(),data[k].value);}
		putchar('\n');
		}

		{
		auto reader=dump.arrayGet<SnowflakeModel::DataDump::StringHolder>("str2");
		auto data=reader.dataRead();
		auto n=data.size();
		printf("Got %zu values:\n",n);
		for(size_t k=0;k<n;++k)
			{printf("    %s\n",data[k].begin());}
		putchar('\n');
		}
	

	return 0;
	}