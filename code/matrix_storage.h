//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"matrix_storage.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
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
#include <cstdint>
#include <cassert>
#include <cstddef>
#include <vector>

namespace SnowflakeModel
	{
	class Thread;

	class MatrixStorage
		{
		public:
			typedef double ElementType;

			static size_t N_validate(size_t N,size_t M);

			MatrixStorage(size_t N_rows,size_t N_cols);
			~MatrixStorage();

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

			ElementType sumGet() const
				{
				if(m_flags_dirty&SUM_DIRTY)
					{sumCompute();}
				return m_sum;
				}

			ElementType sumGetMt() const
				{
				if(m_flags_dirty&SUM_DIRTY)
					{sumComputeMt();}
				return m_sum;
				}

			const double* sumsRowBegin() const
				{
				return m_sums_row.data();
				}

			const double* sumsRowEnd() const
				{
				return m_sums_row.data()+m_sums_row.size();
				}


		private:
			typedef ElementType vec4_t __attribute__ ( (vector_size(4*sizeof(double))) );

			size_t m_N_cols;
			std::vector<ElementType> m_data;

			mutable ElementType m_sum;

			class SumTask;
			mutable std::vector<double> m_sums_row;
			mutable std::vector<SumTask> m_sums;
			mutable std::vector<Thread> m_sum_workers;
			mutable uint32_t m_flags_dirty;
			static constexpr uint32_t SUM_DIRTY=1;

			void sumCompute() const;
			void sumComputeMt() const;



		};
	}

#endif
