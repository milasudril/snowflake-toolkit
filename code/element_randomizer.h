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
#include <algorithm>

namespace SnowflakeModel
	{
	template<class ElementType>
	Twins<size_t> elementChoose(RandomGenerator& randgen,const MatrixStorage<ElementType>& M) noexcept
		{
		std::uniform_real_distribution<double> U(0, M.sumGetMt());
		auto r=U(randgen);
		auto blocksums=M.blocksumsBegin();
		auto block=std::upper_bound(blocksums,M.blocksumsEnd(),r) - 1;
		if(block + 1==M.blocksumsEnd())
			{
			fprintf(stderr,"# ERROR: No match. This should not happen. The probability matrix may have a too large range\n");
			abort();
			}
		
		auto diff=*block;
		auto offset=block-blocksums;
		auto res=std::upper_bound(M.cumsumBegin(offset)
			,M.cumsumEnd(block-blocksums),r,[diff](decltype(r) value,ElementType e)
				{
				return value<e + diff;
				});

		if(res==M.cumsumEnd(offset))
			{
			fprintf(stderr,"# ERROR: No match. This should not happen. The probability matrix may have a too large range\n");
			abort();
			}
		
		auto pos=(res-M.cumsumBegin(0));
		return M.locationGet(pos);
		}
	}

#endif
