//@	{"targets":[{"name":"matrixsumtest","type":"application"}]}

#include "matrix_storage.h"
#include "element_randomizer.h"
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
			m(k,l)=k*n_cols + l + 0;
			printf("%d ",m(k,l));
			}
		putchar('\n');
		}

	printf("Total: %d\n\n",m.sumGetMt());

		{
		auto ptr=m.blocksumsBegin();
		auto ptr_end=m.blocksumsEnd();
		while(ptr!=ptr_end)
			{
			printf("%d  ",*ptr);
			++ptr;
			}
		printf("\n\n");
		}


		{
		SnowflakeModel::MatrixStorage<size_t> m_2(7,5);
		SnowflakeModel::RandomGenerator randgen;
		const int N=1<<29;
		for(int k=0;k<N;++k)
			{
			auto e=SnowflakeModel::elementChoose(randgen,m);
			m_2(e.first,e.second)+=1;
			}

		for(size_t k=0;k<n_rows;++k)
			{
			for(size_t l=0;l<n_cols;++l)
				{
				printf("%.7g ",m.sumGetMt()*(m_2(k,l)/static_cast<double>(N)));
				}
			putchar('\n');
			}
		}
	return 0;
	}
