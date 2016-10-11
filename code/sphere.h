//@	{
//@	 "targets":[{"name":"sphere.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"sphere.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_SPHERE_H
#define SNOWFLAKEMODEL_SPHERE_H

#include "vector.h"
#include "bounding_box.h"
#include <cmath>

namespace SnowflakeModel
	{
	class Grid;
	class Sphere
		{
		public:
			explicit Sphere(const Point& location,float radius):
				m_radius(radius),m_location(location)
				{}

			void geometrySample(Grid& grid) const;

			bool inside(const Point& p) const noexcept
				{
				auto v=p-m_location;
				auto r=m_radius;
				return glm::dot(v,v)<=r*r;
				}

			const Point& midpointGet() const noexcept
				{return m_location;}

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

			float shoot(const Point& source
				,const Vector& direction
				,float E_0,float decay_distance) const noexcept
				{
				auto d=Vector(source-m_location);
				auto a=glm::dot(direction,d);
				auto b=glm::dot(d,d);
				auto r2=m_radius;
				r2*=r2;
				
				auto sqr=a*a-b+r2;
				if(sqr<0.0f)
					{return INFINITY;}

				sqr=std::sqrt(sqr);
				auto distance=std::min(-a+sqr, -a-sqr);
				assert(distance>=0.0f);
				auto v=normalize(Vector(source + Point(distance*direction,1.0f) - m_location));
				auto proj=glm::dot(v,direction);
				auto E_out=E_0*(1.0f - proj)
					*std::exp(-distance/decay_distance);

				return E_out<1.0f?distance:INFINITY;
				}

			float radiusGet() const noexcept
				{return m_radius;}

			BoundingBox boundingBoxGet() const noexcept
				{
				auto r=m_radius;
				auto v=Point(r,r,r,0.0f);
				auto pos=m_location;
				return {pos - v,pos + v};
				};

		private:
			float m_radius;
			Point m_location;
		};

	inline bool overlap(const Sphere& a, const Sphere& b)
		{
		auto v=a.midpointGet() - b.midpointGet();
		auto r_tot=a.radiusGet() + b.radiusGet();
		return glm::dot(v,v) <= r_tot*r_tot;
		}
	}

#endif
