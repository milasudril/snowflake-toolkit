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

	inline bool intersect(const BoundingBox& bb,const Point& source,const Vector& direction)
		{
	//	Adopted from Gamedev
	//		http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

		auto dirfrac=1.0f/direction;

		float t1 = (bb.m_min.x - source.x)*dirfrac.x;
		float t3 = (bb.m_min.y - source.y)*dirfrac.y;
		float t5 = (bb.m_min.z - source.z)*dirfrac.z;
		float t2 = (bb.m_max.x - source.x)*dirfrac.x;
		float t4 = (bb.m_max.y - source.y)*dirfrac.y;
		float t6 = (bb.m_max.z - source.z)*dirfrac.z;

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behind us
		if (tmax < 0)
			{return 0;}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
			{return 0;}
		return 1;
		}
	}


#endif
