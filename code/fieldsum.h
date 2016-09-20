//@	{
//@	"targets":[{"name":"fieldsum.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"fieldsum.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_FIELDSUM_H
#define SNOWFLAKEMODEL_FIELDSUM_H

#include "vector.h"

namespace SnowflakeModel
	{
	class Triangle;
	
	double flowIntegrate(const Point& source,const Triangle& triangle);
	}

#endif