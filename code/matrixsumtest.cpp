//@	{"targets":[{"name":"matrixsumtest","type":"application"}]}

#include "matrix_storage.h"
#include <cstdio>

int main()
	{
	SnowflakeModel::MatrixStorage<int> m(7,5);

	auto n_rows=m.nRowsGet();
	auto n_cols=m.nColsGet();
		
	for(size_t k=0;k<n_rows;++k)
		{
		for(size_t l=0;l<n_cols;++l)
			{
			m(k,l)=k*n_cols + l + 1;
			printf("%d ",m(k,l));
			}
		putchar('\n');
		}

	printf("\nTotal: %d\n\n",m.sumGetMt());

	printf("Blocksums: ");
	auto ptr=m.blocksumsBegin();
	auto ptr_end=m.blocksumsEnd();
	while(ptr!=ptr_end)
		{
		printf("%d ",*ptr);
		++ptr;
		}
	putchar('\n');

	printf("\nCumsum: ");
	ptr=m.cumsumBegin(0);
	ptr_end=m.cumsumEnd();
	while(ptr!=ptr_end)
		{
		printf("%d ",*ptr);
		++ptr;
		}
	putchar('\n');



	return 0;
	}
