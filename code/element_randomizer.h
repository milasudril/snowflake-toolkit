//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"element_randomizer.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
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
#include <vector>

namespace SnowflakeModel
	{
	class MatrixStorageFastsum;

	Twins<size_t> elementChoose(RandomGenerator& randgen,const MatrixStorageFastsum& M) noexcept;
	}

#endif
