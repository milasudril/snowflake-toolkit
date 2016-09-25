//@	{
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"matrix_storage.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_MATRIXSTORAGE_H
#define SNOWFLAKEMODEL_MATRIXSTORAGE_H

#include "twins.h"
#include "thread.h"
#include <cstdint>
#include <cassert>
#include <cstddef>
#include <vector>
#include <memory>
#include <cstdlib>

#include <cstdio>

namespace SnowflakeModel
	{
	template<class ElementType>
	class MatrixStorage
		{
		public:
			MatrixStorage(size_t N_rows,size_t N_cols);

			const ElementType& operator()(size_t row,size_t col) const
				{return m_data[m_N_cols*row + col];}

			ElementType& operator()(size_t row,size_t col)
				{
				m_flags_dirty|=SUM_DIRTY;
				return m_data[m_N_cols*row + col];
				}

			void symmetricAssign(size_t row,size_t col,const ElementType& value)
				{
				assert(row<nColsGet() && col<nRowsGet() && nRowsGet()==nColsGet());
			//	TODO: Cache performance!!!
				(*this)(row,col)=value;
				(*this)(col,row)=value;
				}

			const ElementType* rowGet(size_t row) const
				{return &m_data[m_N_cols*row];}

			ElementType* rowGet(size_t row)
				{
				m_flags_dirty|=SUM_DIRTY;
				return &m_data[m_N_cols*row];
				}

			size_t sizeGet() const
				{return m_data.size();}

			size_t nColsGet() const
				{return m_N_cols;}

			size_t nRowsGet() const
				{return sizeGet()/nColsGet();}

			const ElementType* rowsEnd() const
				{return m_data.data()+sizeGet();}

			ElementType* rowsEnd()
				{return m_data.data()+sizeGet();}

			Twins<size_t> locationGet(size_t index) const
				{
				auto row=index/m_N_cols;
				auto col=index%m_N_cols;
				return {row,col};
				}

			ElementType sumGetMt() const
				{
				if(m_flags_dirty&SUM_DIRTY)
					{sumComputeMt();}
				return m_sum;
				}

		private:
		//	We want this later...
		//	typedef ElementType vec4_t __attribute__ ( (vector_size(4*sizeof(ElementType))) );

			size_t m_N_cols;
			std::vector<ElementType> m_data;
			mutable ElementType m_sum;

			mutable uint32_t m_flags_dirty;
			static constexpr uint32_t SUM_DIRTY=1;
			void sumComputeMt() const;
		};

	template<class ElementType>
	MatrixStorage<ElementType>::MatrixStorage(size_t N_rows,size_t N_cols):m_N_cols(N_cols)
		,m_data(N_rows*N_cols),m_sum(0),m_flags_dirty(0)
		{
		}

	namespace
		{
		template<class ElementType>
		class SumTask
			{
			public:
				SumTask(const ElementType* ptr_begin,const ElementType* ptr_end):
					pos_begin(ptr_begin),pos_end(ptr_end)
					{}

				const ElementType& resultGet() const noexcept
					{return m_result;}

				void operator()() noexcept
					{
					auto ptr=pos_begin;
					auto ptr_end=pos_end;
					auto sum=static_cast<ElementType>( 0 );
					while(ptr!=ptr_end)
						{
						sum+=*ptr;
						++ptr;
						}
					m_result=sum;
					}
		

			private:
				const ElementType* pos_begin;
				const ElementType* pos_end;
				ElementType m_result;
			};
		}

	namespace
		{
		struct free_delete
			{
			void operator()(void* x) { free(x); }
			};
		}

	template<class ElementType>
	void MatrixStorage<ElementType>::sumComputeMt() const
		{
		auto n_threads=threadsCountGet();
		auto L=m_data.size()/n_threads;
		int mod=m_data.size()%n_threads;

		typedef Thread<SumTask<ElementType>> SumThread;
		
		std::unique_ptr<SumThread,free_delete> threads
			{reinterpret_cast<SumThread*>(malloc( sizeof(SumThread)*n_threads ) ),free_delete() };

		auto pos_begin=m_data.data();
		auto pos_threads=threads.get();
		for(decltype(n_threads) k=0;k<n_threads;++k)
			{
			auto next=L + std::min(mod,1);
			new(pos_threads)SumThread( SumTask<ElementType>{pos_begin,pos_begin + next},k ); 
			pos_begin+=next;
			++pos_threads;
			mod=std::max(mod - 1, 0);
			}
		
		auto sum=static_cast<ElementType>( 0 );
		for(decltype(n_threads) k=0;k<n_threads;++k)
			{
			--pos_threads;
		//	This call implies complete destruction. This is due to the fact that
		//	SumTask is trivially destructible
			sum+=pos_threads->taskGet().resultGet();
			}

		m_sum=sum;
		}
	}

#endif
