//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"bounding_box.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_BOUNDINGBOX_H
#define SNOWFLAKEMODEL_BOUNDINGBOX_H

#include "vector.h"

namespace SnowflakeModel
	{
	struct BoundingBox
		{
		Point m_min;
		Point m_max;

		Point centerGet() const noexcept
			{return 0.5f*(m_min + m_max);}

		};

	inline bool overlap(const BoundingBox& a,const BoundingBox& b) noexcept
		{
		auto size_tot=a.m_max - a.m_min
			+ b.m_max - b.m_min; //largest possible distance for overlap
		auto distance=glm::abs(a.m_min + a.m_max
			- b.m_min - b.m_max); //twice the distance between the two bounding box centers
		
		return distance.x<size_tot.x 
			|| distance.y<size_tot.y
			|| distance.z<size_tot.z;
		}

	inline bool inside(const Point& p,const BoundingBox& box)
		{
		return p.x>=box.m_min.x && p.x<=box.m_max.x
			&& p.x>=box.m_min.y && p.x<=box.m_max.y
			&& p.z>=box.m_min.z && p.z<=box.m_max.z;
		}
	}


#endif
