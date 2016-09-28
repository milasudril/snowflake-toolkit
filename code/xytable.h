//@	{
//@	"targets":[{"name":"xytable.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"xytable.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_XYTABLE_H
#define SNOWFLAKEMODEL_XYTABLE_H

#include "twins.h"
#include <vector>

namespace SnowflakeModel
	{
	class FileIn;
	std::vector< Twins<float> > xytable(FileIn& source);
	}

#endif