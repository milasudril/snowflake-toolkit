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
#include "matrix_storage.h"
#include "thread.h"
#include "profile.h"

using namespace SnowflakeModel;

ElementRandomizer::ElementRandomizer(const MatrixStorage<double>& M):r_M(M)
	{
	}

ElementRandomizer::~ElementRandomizer()
	{}

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

Twins<size_t> ElementRandomizer::elementChoose(RandomGenerator& randgen) const noexcept
	{
	SNOWFLAKEMODEL_TIMED_SCOPE();

	size_t i_row;
	double sum_row;
		{
		std::uniform_real_distribution<double> U(0,r_M.sumGetMt());
		auto r=U(randgen);

		const double* ptr_sum;
		do
			{ptr_sum=element_choose(r_M.sumsRowBegin(),r_M.sumsRowEnd(),r);}
		while(!element_valid(ptr_sum,r_M.sumsRowEnd()));

		sum_row=*ptr_sum;
		i_row=ptr_sum-r_M.sumsRowBegin();
		}

	size_t i_col;
		{
		std::uniform_real_distribution<double> U(0,sum_row);
		auto r=U(randgen);
		auto row_begin=r_M.rowGet(i_row);
		auto row_end=row_begin + r_M.nColsGet();

		const double* ptr_col;
		do
			{ptr_col=element_choose(row_begin,row_end,r);}
		while(!element_valid(ptr_col,row_end));

		i_col=ptr_col-row_begin;
		}

	return {i_row,i_col};
	}

#if 0
Twins<size_t> ElementRandomizer::elementChoose(RandomGenerator& randgen)
	{
	SNOWFLAKEMODEL_TIMED_SCOPE();
	do
		{
		m_stop=0;
		std::uniform_real_distribution<double> U(0,r_M.sumGetMt());
		m_r=U(randgen);

		auto ptr=m_tasks.data();
		auto end=m_tasks.data()+m_tasks.size();
		uint32_t k=0;
		while(ptr!=end)
			{
			m_workers.push_back(Thread{*ptr,k});
			++ptr;
			++k;
			}
		m_workers.clear();

		ptr=m_tasks.data();
		while(ptr!=end)
			{
			if(*ptr!=ptr->endGet())
				{
				return r_M.locationGet(*ptr - r_M.rowGet(0));
				}
			++ptr;
			}
		fprintf(stderr,"# WARNING: No element matched. Trying again.\n");
		}
	while(1);
	}

#endif


