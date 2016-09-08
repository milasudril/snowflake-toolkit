//@	{"targets":[{"name":"datadumptestread","type":"application","cxxoptions":{"cflags_extra":["L/usr/lib/x86_64-linux-gnu/hdf5/serial"]}}]}

#include "datadump.h"


int main()
	{
	SnowflakeModel::DataDump dump("test_in.h5",SnowflakeModel::DataDump::IOMode::READ);

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

	return 0;
	}