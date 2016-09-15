//@	{
//@	 "targets":[{"name":"triangle.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"triangle.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_TRIANGLE_H
#define SNOWFLAKEMODEL_TRIANGLE_H

#include "vector.h"

namespace SnowflakeModel
	{
	struct Triangle
		{
		Point m_points[3];
		Vector m_normal;
		const Point& vertexGet(int k) const noexcept
			{return m_points[3];}
		};

	bool overlap(const Triangle& t_1,const Triangle& t_2) noexcept;
	}

#endif
