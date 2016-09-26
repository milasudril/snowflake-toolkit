//@	{"targets":[{"name":"matrixsumtest","type":"application"}]}

#include "matrix_storage.h"
#include "element_randomizer.h"
#include "pmap_loader.h"
#include <cstdio>

int main()
	{
	try
		{
		auto m=SnowflakeModel::pmapLoad("test2.png");

		auto n_rows=m.nRowsGet();
		auto n_cols=m.nColsGet();
			
		for(size_t k=0;k<n_rows;++k)
			{
			for(size_t l=0;l<n_cols;++l)
				{
				printf("%.7g ",m(k,l));
				}
			putchar('\n');
			}

		printf("Total: %.7g\n\n",m.sumGetMt());

			{
			auto ptr=m.blocksumsBegin();
			auto ptr_end=m.blocksumsEnd();
			while(ptr!=ptr_end)
				{
				printf("%.7g  ",*ptr);
				++ptr;
				}
			printf("\n\n");
			}
		}
	catch(const char* msg)
		{fprintf(stderr,"Error: %s",msg);}

	return 0;
	}
