//@	{"targets":[{"name":"matrixsumtest","type":"application"}]}

#include "matrix_storage.h"
#include <cstdio>

int main()
	{
	SnowflakeModel::MatrixStorage<int> m(4,4);

	auto n_rows=m.nRowsGet();
	auto n_cols=m.nColsGet();
		
	for(size_t k=0;k<n_rows;++k)
		{
		for(size_t l=0;l<n_cols;++l)
			{
			m(k,l)=k*n_cols + l;
			}
		}

	printf("%d\n",m.sumGetMt());

	return 0;
	}
