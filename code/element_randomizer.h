//@	{
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"element_randomizer.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_ELEMENTRANDOMIZER_H
#define SNOWFLAKEMODEL_ELEMENTRANDOMIZER_H

#include "twins.h"
#include "randomgenerator.h"
#include "matrix_storage.h"

namespace SnowflakeModel
	{
	namespace
		{
		template<class ElementType>
		const ElementType* element_choose(const ElementType* begin,const ElementType* end
			,double r)
			{
			auto sum=static_cast<ElementType>(0);
			while(begin!=end)
				{
				if(r>=sum && r<*begin + sum)
					{return begin;}
				sum+=*begin;
				++begin;
				}
			return end;
			}
		}


	template<class ElementType>
	Twins<size_t> elementChoose(RandomGenerator& randgen,const MatrixStorage<ElementType>& M) noexcept
		{
		std::uniform_real_distribution<double> U(0, M.sumGetMt());
		auto ptr_begin=M.rowGet(0);
		auto ptr_end=ptr_begin + M.sizeGet();

		auto res=element_choose(ptr_begin,ptr_end,U(randgen));

		if(res==ptr_end)
			{
			fprintf(stderr,"# ERROR: No match. This should not happen. The probability matrix may have a too large range\n");
			abort();
			}

		return M.locationGet(res-ptr_begin);
		}
	}

#endif
