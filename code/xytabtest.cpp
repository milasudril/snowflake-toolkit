//@	{"targets":[{"name":"xytabtest","type":"application"}]}

#include "xytable.h"
#include "file_in.h"
#include "stride_iterator.h"

int main()
	{
	try
		{
		auto table=SnowflakeModel::xytable(SnowflakeModel::FileIn("xytab.txt"));

		SnowflakeModel::StrideIterator<decltype(table)::value_type,0> x(table.data());
		SnowflakeModel::StrideIterator<decltype(table)::value_type,1> y(table.data());

		size_t N=table.size();
		while(N!=0)
			{
			printf("%.7g\t",*x);
			++x;
			--N;
			}
		putchar('\n');
		N=table.size();
		while(N!=0)
			{
			printf("%.7g\t",*y);
			++y;
			--N;
			}

		return 0;
		}
	catch(const char* msg)
		{
		fprintf(stderr,"Error: %s\n",msg);
		}

	}
