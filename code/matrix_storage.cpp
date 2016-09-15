//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[
//@	                {
//@	                    "ref":"new.o",
//@	                    "rel":"implementation"
//@	                }
//@	            ],
//@	            "name":"matrix_storage.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "matrix_storage.h"
#include "thread.h"
#include "task.h"
#include "profile.h"
#include <cstring>

using namespace SnowflakeModel;

size_t MatrixStorage::N_validate(size_t N_rows,size_t N_cols)
	{
	if(N_cols%4!=0)
		{throw "Number of columns has to be divisible by 4";}
//	TODO use more flexible division. Simple but not optimal:
//	N/c + N/c + N/c + ... + remaining rows
//	Improve by moving the extra rows on to other tasks?
	if(N_rows%Thread::threadsMax())
		{throw "Number of rows has to be divisible by the number of cores";}

	return N_rows*N_cols;
	}

class MatrixStorage::SumTask:public Task
	{
	public:
		SumTask(const ElementType* begin,const ElementType* end
			,ElementType* row_sum
			,size_t N_cols):
			r_begin(begin),r_end(end),r_row_sum(row_sum),m_N_cols(N_cols/4)
			{
			if( ((intptr_t)begin%64) != 0)
				{throw "Bad memory alignment";}
			}

		void run() noexcept
			{
			assert((r_end-r_begin)%4==0);

			auto ptr=(const vec4_t*)r_begin;
			auto end=(const vec4_t*)r_end;
			m_sum=0;
			auto ptr_row_sum=r_row_sum;
			while(ptr!=end)
				{
				size_t N=m_N_cols;
				vec4_t sum4={0.0,0.0,0.0,0.0};
				while(N)
					{
					--N;
					sum4+=*ptr;
					++ptr;
					}
				double row_sum=sum4[0]+sum4[1]+sum4[2]+sum4[3];
				m_sum+=row_sum;
				*ptr_row_sum=row_sum;
				++ptr_row_sum;
				}
			}

		operator double() const
			{return m_sum;}

	private:
		const ElementType* r_begin;
		const ElementType* r_end;
		ElementType* r_row_sum;
		size_t m_N_cols;
		ElementType m_sum;
	};

MatrixStorage::MatrixStorage(size_t N_rows,size_t N_cols):
	m_N_cols(N_cols)
	,m_data(N_validate(N_rows,N_cols))
	,m_sums_row(N_rows),m_flags_dirty(SUM_DIRTY)
	{
	memset(m_data.data(),0,sizeof(ElementType)*m_data.size());
	size_t n_threads=Thread::threadsMax();
	auto n_rows=N_rows/n_threads;
	for(decltype(n_threads) k=0; k<n_threads;++k)
		{
		m_sums.push_back(
			{rowGet(k*n_rows), rowGet( (k+1)*n_rows) ,&m_sums_row[k*n_rows],N_cols}
			);
		}
	}

MatrixStorage::~MatrixStorage()
	{}

void MatrixStorage::sumCompute() const
	{
	auto ptr=(const vec4_t*)rowGet(0);
	auto end=(const vec4_t*)rowsEnd();
	vec4_t sum4={0.0,0.0,0.0,0.0};
	while(ptr!=end)
		{
		sum4+=*ptr;
		++ptr;
		}
	m_sum=sum4[0]+sum4[1]+sum4[2]+sum4[3];
	m_flags_dirty&=~SUM_DIRTY;
	}

void MatrixStorage::sumComputeMt() const
	{
	SNOWFLAKEMODEL_TIMED_SCOPE();
		{
		auto ptr=m_sums.data();
		auto end=ptr+m_sums.size();
		uint32_t k=0;
		while(ptr!=end)
			{
			m_sum_workers.push_back(Thread{*ptr,k});
			++ptr;
			++k;
			}
		m_sum_workers.clear();
		}

		{
		m_sum=0;
		auto ptr=m_sums.data();
		auto end=ptr+m_sums.size();
		while(ptr!=end)
			{
			m_sum+=(double)(*ptr);
			++ptr;
			}
		}
	m_flags_dirty&=~SUM_DIRTY;
	}
