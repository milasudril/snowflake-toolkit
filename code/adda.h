//@ {
//@	 "targets":[{"name":"adda.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"adda.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_ADDA_H
#define SNOWFLAKEMODEL_ADDA_H

namespace SnowflakeModel
	{
	class Grid;
	class FileOut;
	void addaShapeWrite(const Grid& grid,FileOut&& dest);
	}

#endif
