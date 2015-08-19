#ifdef __WAND__
target[name[mt_test] type[application]]
#endif

#include "matrix_storage.h"
#include "element_randomizer.h"
#include <random>

double U(double a,double b,std::mt19937& randgen)
	{
	return std::uniform_real_distribution<double>(a,b)(randgen);
	}

template<class T>
size_t randomDraw(double sum,const T* ptr_begin,const T* ptr_end
	,T p_mass,std::mt19937& randgen)
	{
	do
		{
		auto r=U(static_cast<T>(0),static_cast<T>(p_mass),randgen);
		auto ptr=ptr_begin;
		while(ptr!=ptr_end)
			{
			if(r>=sum && r<*ptr + sum)
				{return ptr-ptr_begin;}
			sum+=*ptr;
			++ptr;
			}
		fprintf(stderr,"# WARNING: no event matched. Trying again.\n");
		}
	while(1);
	}

SnowflakeModel::Twins<size_t>
grainsChoose(const SnowflakeModel::MatrixStorage& C_mat
	,std::mt19937& randgen)
	{
	auto i=randomDraw(0,C_mat.rowGet(0),C_mat.rowsEnd(),C_mat.sumGet(),randgen);
	return C_mat.locationGet(i);
	}

int main()
	{
	try
		{
		SnowflakeModel::MatrixStorage M(5000,5000);
		std::fill(M.rowGet(0),M.rowsEnd(),1);
		std::mt19937 rng;
		rng.seed(0);
		SnowflakeModel::Twins<size_t> elems{0,0};
		SnowflakeModel::ElementRandomizer randomizer(M);
		auto t0=clock();
		for(size_t k=0;k<32768;++k)
			{
			elems=randomizer.elementChoose(rng);
			printf("%zu %zu\n",elems.first,elems.second);
			}
		auto time=double(clock()-t0)/(CLOCKS_PER_SEC);
		fprintf(stderr,"Time mt: %.15g Elems=(%zu,%zu)\n",time/4,elems.first,elems.second);
		printf("=========================\n");
		rng.seed(0);
		t0=clock();
		for(size_t k=0;k<32768;++k)
			{
			elems=grainsChoose(M,rng);
			printf("%zu %zu\n",elems.first,elems.second);
			}
		time=double(clock()-t0)/(CLOCKS_PER_SEC);
		fprintf(stderr,"Time mt: %.15g Elems=(%zu,%zu)\n",time,elems.first,elems.second);
		}
	catch(const char* msg)
		{fprintf(stderr,"%s\n",msg);return -1;}

	return 0;
	}
