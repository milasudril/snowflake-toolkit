//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"element_randomizer.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "element_randomizer.h"
#include "task.h"
#include "matrix_storage_fastsum.h"
#include "profile.h"

using namespace SnowflakeModel;


namespace
	{
	const double* element_choose(const double* begin,const double* end,double r)
		{
		double sum=0;
		while(begin!=end)
			{
			if(r>=sum && r<*begin + sum)
				{return begin;}
			sum+=*begin;
			++begin;
			}
		return end;
		}

	bool element_valid(const double* element,const double* end)
		{
		if(element==end)
			{
			fprintf(stderr,"# WARNING: No match\n");
			abort();
			return 0;
			}
		return 1;
		}
	}

Twins<size_t> SnowflakeModel::elementChoose(RandomGenerator& randgen,const MatrixStorageFastsum& M) noexcept
	{
	SNOWFLAKEMODEL_TIMED_SCOPE();

	size_t i_row;
	double sum_row;
		{
		std::uniform_real_distribution<double> U(0,M.sumGetMt());
		auto r=U(randgen);

		const double* ptr_sum;
		do
			{ptr_sum=element_choose(M.sumsRowBegin(),M.sumsRowEnd(),r);}
		while(!element_valid(ptr_sum,M.sumsRowEnd()));

		sum_row=*ptr_sum;
		i_row=ptr_sum-M.sumsRowBegin();
		}

	size_t i_col;
		{
		std::uniform_real_distribution<double> U(0,sum_row);
		auto r=U(randgen);
		auto row_begin=M.rowGet(i_row);
		auto row_end=row_begin + M.nColsGet();

		const double* ptr_col;
		do
			{ptr_col=element_choose(row_begin,row_end,r);}
		while(!element_valid(ptr_col,row_end));

		i_col=ptr_col-row_begin;
		}

	return {i_row,i_col};
	}
