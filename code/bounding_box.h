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
#include <random>

namespace SnowflakeModel
	{
	struct BoundingBox
		{
		Point m_min;
		Point m_max;

		Point centerGet() const noexcept
			{return 0.5f*(m_min + m_max);}

		Vector sizeGet() const noexcept
			{return Vector(m_max-m_min);}

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
			&& p.y>=box.m_min.y && p.y<=box.m_max.y
			&& p.z>=box.m_min.z && p.z<=box.m_max.z;
		}


	template<class Rng>
	Point randomPoint(const BoundingBox& box,Rng& rng)
		{
		Point ret;
		ret.x=std::uniform_real_distribution<float>(box.m_min.x,box.m_max.x)(rng);
		ret.y=std::uniform_real_distribution<float>(box.m_min.y,box.m_max.y)(rng);
		ret.z=std::uniform_real_distribution<float>(box.m_min.z,box.m_max.z)(rng);
		ret.w=1.0f;
		return ret;
		}

	inline float distance(const Point& p,const BoundingBox& box)
		{
		return glm::length( glm::max(box.m_min - p,p - box.m_max) );
		}
	}


#endif
