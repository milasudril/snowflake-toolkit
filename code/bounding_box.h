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
		Vector m_min;
		Vector m_max;
		};

	inline bool overlap(const BoundingBox& a,const BoundingBox& b) noexcept
		{
		auto size_tot=a.m_max - a.m_min
			+ b.m_max - b.m_min; //largest possible distance for overlap
		auto distance=glm::abs(a.m_min + a.m_max
			- b.m_min - b.m_max); //twice the distance between the two bounding box centers
		
		return distance.x<=size_tot.x 
			&& distance.y<=size_tot.y
			&& distance.z<=size_tot.z;
		}
	}

#endif
