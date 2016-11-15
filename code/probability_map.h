//@	{
//@	"targets":[{"name":"probability_map.h","type":"include"}]
//@	}
#ifndef SNOWFLAKEMODEL_PROBABILITYMAP_H
#define SNOWFLAKEMODEL_PROBABILITYMAP_H

#include "twins.h"
#include "randomgenerator.h"
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cstddef>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <cstdio>

namespace SnowflakeModel
	{
	template<class ElementType>
	class ProbabilityMap
		{
		public:
			ProbabilityMap():ProbabilityMap(1,1){}

			explicit ProbabilityMap(uint32_t N_rows,uint32_t N_cols);

			Twins<uint32_t> elementGet(RandomGenerator& rng) const noexcept;

			auto nRowsGet() const noexcept
				{return m_N_rows;}

			auto nColsGet() const noexcept
				{return m_N_cols;}

			auto sumGet() const noexcept
				{
				if(m_sum_dirty)
					{sumCompute();}
				return m_sum;
				}

			auto rowGet(uint32_t row) noexcept
				{
				m_sum_dirty=1;
				return m_data.get() + row*m_N_cols;
				}

			auto rowGet(uint32_t row) const noexcept
				{return m_data.get() + row*m_N_cols;}

		private:
			struct free_delete
				{ void operator()(void* x) { free(x); } };

			uint32_t m_N_rows;
			uint32_t m_N_cols;
			mutable double m_sum;
			std::unique_ptr<ElementType,free_delete> m_data;
			std::unique_ptr<double,free_delete> m_cumsum;
			mutable bool m_sum_dirty;

			void sumCompute() const noexcept;
		};

	template<class ElementType>
	ProbabilityMap<ElementType>::ProbabilityMap(uint32_t N_rows,uint32_t N_cols):
		m_N_rows(N_rows),m_N_cols(N_cols),m_sum(0)
		{
		auto N_bytes=N_rows*N_cols*sizeof(ElementType);
		
		m_data.reset(reinterpret_cast<ElementType*>(malloc(N_bytes)));
		std::fill(m_data.get(),m_data.get() + N_rows*N_cols,1);

		m_cumsum.reset(reinterpret_cast<double*>(malloc(N_cols*N_rows*sizeof(double))));
		memset(m_cumsum.get(),0,N_cols*N_rows*sizeof(double));
		m_sum_dirty=1;
		}

	template<class ElementType>
	void ProbabilityMap<ElementType>::sumCompute() const noexcept
		{
		double sum=0;
		auto ptr_cumsum=m_cumsum.get();
		auto ptr_end=ptr_cumsum + m_N_rows*m_N_cols;
		auto ptr_data=m_data.get();
		while(ptr_cumsum!=ptr_end)
			{
			sum+=*ptr_data;
			*ptr_cumsum=sum;
			++ptr_data;
			++ptr_cumsum;
			}
		m_sum=sum;
		m_sum_dirty=0;
		}

	template<class ElementType>
	Twins<uint32_t> ProbabilityMap<ElementType>::elementGet(RandomGenerator& rng) const noexcept
		{
		auto U=std::uniform_real_distribution<double>(0,sumGet());
		auto r=U(rng);
		auto ptr=m_cumsum.get();
		auto ptr_end=ptr + m_N_rows*m_N_cols;
		auto pos=std::lower_bound(ptr,ptr_end,r
			,[](decltype(r) value,double limit)
				{return value<limit;}
			);
		if(pos==ptr_end)
			{
			fprintf(stderr,"# ERROR: No match. This should not happen. The probability matrix may have a too large range\n");
			abort();
			}
		auto offset=pos - ptr;
		auto row=offset/m_N_cols;
		auto col=offset%m_N_cols;
		return Twins<uint32_t>(row,col);
		}

	template<class ElementType>
	auto elementChoose(RandomGenerator& rng,const ProbabilityMap<ElementType>& e)
		{
		return e.elementGet(rng);
		}
	}

#endif