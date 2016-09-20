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
			{return m_points[k];}

		Point midpointGet() const noexcept
			{
			auto ret=m_points[0] + m_points[1] + m_points[2];
			ret.w=1.0f;
			return ret;
			}
		};

	bool overlap(const Triangle& t_1,const Triangle& t_2) noexcept;
	bool intersects(const Triangle& T,const Point& origin
		,const Vector& direction,float& out) noexcept;
	}

#endif
