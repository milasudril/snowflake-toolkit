//@	{
//@	"targets":
//@		[{
//@		"name":"datadumptestread","type":"application"
//@		,"description":"Test application for DataDump (Read)"
//@		}]
//@	}

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


		{
		auto reader=dump.arrayRead<Foo>("obj");
		auto data=reader.dataRead();
		auto n=data.size();
		printf("Got %zu values:\n",n);
		for(size_t k=0;k<n;++k)
			{printf("    %s %d",data[k].key.begin(),data[k].value);}
		putchar('\n');
		}

		{
		auto reader=dump.arrayRead<SnowflakeModel::DataDump::StringHolder>("str2");
		auto data=reader.dataRead();
		auto n=data.size();
		printf("Got %zu values:\n",n);
		for(size_t k=0;k<n;++k)
			{printf("    %s\n",data[k].begin());}
		putchar('\n');
		}

		{
		long y[2][3]={{0,0,0},{0,0,0}};
		dump.matrixGet<long>("y",&y[0][0],2,3);

		for(int k=0;k<2;++k)
			{
			for(int l=0;l<3;++l)
				{printf("%ld ",y[k][l]);}
			putchar('\n');
			}

		auto data=dump.matrixGet<long>("y");
		printf("%zu %zu\n",data.first.size(),data.second);
		}
	

	return 0;
	}