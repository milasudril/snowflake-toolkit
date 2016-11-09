//@ {
//@	 "targets":[{"name":"adda.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"adda.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_ADDA_H
#define SNOWFLAKEMODEL_ADDA_H

#include "vector.h"
#include "bounding_box.h"

namespace SnowflakeModel
	{
	class Grid;
	class FileOut;
	class FileIn;
	void addaShapeWrite(const Grid& grid,FileOut&& dest);

	std::vector<Vector> pointsLoad(FileIn&& source);
	BoundingBox boundingBoxGet(const std::vector<Vector>& points);
	}

#endif
