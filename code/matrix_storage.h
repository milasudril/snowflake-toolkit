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
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cstddef>
#include <memory>
#include <cstdlib>

#include <cstdio>

namespace SnowflakeModel
	{
	template<class ElementType>
	class MatrixStorage
		{
		public:
			MatrixStorage()=default;

			MatrixStorage(uint32_t N_rows,uint32_t N_cols);

			const ElementType& operator()(size_t row,size_t col) const
				{return ( m_data.get() )[m_N_cols*row + col];}

			ElementType& operator()(size_t row,size_t col)
				{
				m_flags_dirty|=SUM_DIRTY;
				return ( m_data.get() ) [m_N_cols*row + col];
				}

			void symmetricAssign(size_t row,size_t col,const ElementType& value)
				{
				assert(row<nColsGet() && col<nRowsGet() && nRowsGet()==nColsGet());
			//	TODO: Cache performance!!!
				(*this)(row,col)=value;
				(*this)(col,row)=value;
				}

			const ElementType* rowGet(size_t row) const
				{return &(m_data.get())[m_N_cols*row];}

			ElementType* rowGet(size_t row)
				{
				m_flags_dirty|=SUM_DIRTY;
				return &(m_data.get())[m_N_cols*row];
				}

			size_t sizeGet() const
				{return m_N_rows*m_N_cols;}

			size_t nColsGet() const
				{return m_N_cols;}

			size_t nRowsGet() const
				{return m_N_rows;}

			const ElementType* rowsEnd() const
				{return m_data.get() + sizeGet();}

			ElementType* rowsEnd()
				{return m_data.get() + sizeGet();}

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

			const ElementType* blocksumsBegin() const noexcept
				{return m_blocksums.get();}

			const ElementType* blocksumsEnd() const noexcept
				{return blocksumsBegin() + m_N_blocks + 1;}

			const ElementType* cumsumBegin(uint32_t block) const noexcept
				{return (m_blocks.get())[block];}

			const ElementType* cumsumEnd(uint32_t block) const noexcept
				{return (m_blocks.get())[block + 1];}

			void sumComputeMt() const;

		private:
			struct free_delete
				{
				void operator()(void* x) { free(x); }
				};


		//	We want this later...
		//	typedef ElementType vec4_t __attribute__ ( (vector_size(4*sizeof(ElementType))) );
			uint32_t m_N_rows;
			uint32_t m_N_cols;
			std::unique_ptr<ElementType,free_delete> m_data;
			std::unique_ptr<ElementType,free_delete> m_cumsum;
			uint32_t m_N_blocks;
			std::unique_ptr<ElementType*,free_delete> m_blocks;
			std::unique_ptr<ElementType,free_delete> m_blocksums;

			mutable ElementType m_sum;

			mutable uint32_t m_flags_dirty;
			static constexpr uint32_t SUM_DIRTY=1;
		};

	template<class ElementType>
	MatrixStorage<ElementType>::MatrixStorage(uint32_t N_rows,uint32_t N_cols):
		m_N_rows(N_rows),m_N_cols(N_cols),m_sum(0),m_flags_dirty(0)
		{
		auto N_bytes=N_rows*N_cols*sizeof(ElementType);
		
		m_data.reset(reinterpret_cast<ElementType*>(malloc(N_bytes)));
		memset(m_data.get(),0,N_bytes);

		m_cumsum.reset(reinterpret_cast<ElementType*>(malloc(N_bytes)));
		m_N_blocks=threadsCountGet();
		m_blocksums.reset(reinterpret_cast<ElementType*>(malloc((1+m_N_blocks)*sizeof(ElementType))));
		m_blocks.reset(reinterpret_cast<ElementType**>(malloc((1+m_N_blocks)*sizeof(ElementType*))));
		auto L=sizeGet()/m_N_blocks;
		int mod=sizeGet()%m_N_blocks;
		auto p_blocks=m_blocks.get();
		auto cumsum=m_cumsum.get();
		for(decltype(m_N_blocks) k=0;k<m_N_blocks;++k)
			{
			auto next=L + std::min(mod,1);
			*p_blocks=cumsum;
			cumsum+=next;
			mod=std::max(mod - 1, 0);
			++p_blocks;
			}
		*p_blocks=m_cumsum.get() + sizeGet();
		}

	namespace
		{
		template<class ElementType>
		class SumTask
			{
			public:
				SumTask(const ElementType* ptr_begin,const ElementType* ptr_end
					,ElementType* cumsum):
					pos_begin(ptr_begin),pos_end(ptr_end),r_cumsum(cumsum)
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
						*r_cumsum=sum;
						++r_cumsum;
						++ptr;
						}
					m_result=sum;
					}
		

			private:
				const ElementType* pos_begin;
				const ElementType* pos_end;
				ElementType m_result;
				ElementType* r_cumsum;
			};
		}


	template<class ElementType>
	void MatrixStorage<ElementType>::sumComputeMt() const
		{
		auto n_threads=m_N_blocks;
		auto L=sizeGet()/n_threads;
		int mod=sizeGet()%n_threads;

		typedef Thread<SumTask<ElementType>> SumThread;
		
		std::unique_ptr<SumThread,free_delete> threads
			{reinterpret_cast<SumThread*>(malloc( sizeof(SumThread)*n_threads ) ),free_delete() };

		auto pos_begin=m_data.get();
		auto pos_threads=threads.get();
		auto cumsum=m_cumsum.get();
		for(decltype(n_threads) k=0;k<n_threads;++k)
			{
			auto next=L + std::min(mod,1);
			new(pos_threads)SumThread( SumTask<ElementType>{pos_begin,pos_begin + next,cumsum},k ); 
			pos_begin+=next;
			cumsum+=next;
			++pos_threads;
			mod=std::max(mod - 1, 0);
			}
		
		auto sum=static_cast<ElementType>( 0 );
		auto blocksums=m_blocksums.get();
		pos_threads=threads.get();
		auto pos_end=pos_threads + n_threads;
		*blocksums=0;
		++blocksums;
		while(pos_threads!=pos_end)
			{
		//	This call implies complete destruction. This is due to the fact that
		//	SumTask is trivially destructible, so the only thing that needs to
		//	be done is synchronization.
			auto sum_temp=pos_threads->taskGet().resultGet();
			*blocksums=*(blocksums-1) + sum_temp;
			sum+=sum_temp;
			++blocksums;
			++pos_threads;
			}

		m_flags_dirty&=~SUM_DIRTY;
		m_sum=sum;
		}
	}

#endif
