//@	{"targets":[{"name":"sphere.h","type":"include"}]}

#ifndef SNOWFLAKEMODEL_SPHERE_H
#define SNOWFLAKEMODEL_SPHERE_H

#include <cmath>

namespace SnowflakeModel
	{
	class Sphere
		{
		public:
			bool inside(const Point& p) const noexcept
				{
				auto v=p-m_location;
				auto r=m_radius;
				return glm::dot(v,v)<=r*r;
				}

			const Point& midpointGet() const noexcept
				{m_location;}

			float volumeGet() const noexcept
				{
				auto r=m_radius;
				return 4.0f*std::acos(-1.0f)*r*r*r/3.0f;
				}

			float areaVisibleGet() const noexcept
				{
				auto r=m_radius;
				return 4.0f*std::acos(-1.0f)*r*r;
				}

			std::pair<Triangle,float> shoot(const Point& source
				,const Vector& direction
				,float E_0,float decay_distance
				,bool backface_culling) const noexcept;	

			float radiusGet() const noexcept
				{return m_radius;}

		private:
			float m_radius;
			Point m_location;
		};

	bool overlap(const Sphere& a const Sphere& b)
		{
		auto v=a.midpointGet() - b.midpointGet();
		auto r_tot=a.radiusGet() + b.radiusGet();
		return glm::dot(v,v) <= r_tot*r_tot;
		}
	}

#endif
