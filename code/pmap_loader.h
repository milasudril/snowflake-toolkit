//@	{
//@	    "dependencies_extra":[{"ref":"pmap_loader.o","rel":"implementation"}]
//@		,"targets":[{"name":"pmap_loader.h","type":"include"}]
//@	}
#ifndef SNOWFLAKEMODEL_PMAPLOADER_H
#define SNOWFLAKEMODEL_PMAPLOADER_H

#include "probability_map.h"

namespace SnowflakeModel
	{
	SnowflakeModel::ProbabilityMap<float> pmapLoad(const char* filename);
	}

#endif
