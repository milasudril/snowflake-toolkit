//@	{
//@	 "targets":[{"name":"sphere_aggregate.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"sphere_aggregate.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_SPHEREAGGREGATE_H
#define SNOWFLAKEMODEL_SPHEREAGGREGATE_H

#include "sphere.h"

namespace SnowflakeModel
	{
	class Grid;
	class SphereAggregate
		{
		public:
			SphereAggregate():m_bounding_box{{INFINITY,INFINITY,INFINITY,1.0f},{-INFINITY,-INFINITY,-INFINITY,1.0f}}
				{}

			Sphere& subvolumeAdd(const Sphere& volume)
				{
			//	If this fails, the object is in a bad state hmm
			//TODO:	extremaUpdate(volume);
			//****************
				m_subvolumes.push_back(volume);
				m_volume+=volume.volumeGet();
				auto bb=volume.boundingBoxGet();
				m_bounding_box.m_min=glm::min(m_bounding_box.m_min
					,bb.m_min);
				m_bounding_box.m_max=glm::max(m_bounding_box.m_max
					,bb.m_max);
				m_mid+=volume.midpointGet();
				return m_subvolumes.back();
				}

			const Sphere* subvolumesBegin() const noexcept
				{return m_subvolumes.data();}

			const Sphere* subvolumesEnd() const noexcept
				{return m_subvolumes.data()+m_subvolumes.size();}

			Sphere* subvolumesBegin() noexcept
				{return m_subvolumes.data();}

			Sphere* subvolumesEnd() noexcept
				{return m_subvolumes.data()+m_subvolumes.size();}

			size_t subvolumesCount() const noexcept
				{return m_subvolumes.size();}

			const Sphere& subvolumeGet(size_t index) const noexcept
				{return m_subvolumes[index];}

			Sphere& subvolumeGet(size_t index) noexcept
				{return m_subvolumes[index];}

			const BoundingBox& boundingBoxGet() const noexcept
				{return m_bounding_box;}

			Point midpointGet() const noexcept
				{return m_mid/static_cast<float>(subvolumesCount());}

			float volumeGet() const noexcept
				{return m_volume;}

			std::pair<Point,Vector> shoot(const Point& source,const Vector& direction
				,float E_0,float decay_distance) const noexcept;

			void geometrySample(Grid& grid) const;

		private:
			std::vector<Sphere> m_subvolumes;
			BoundingBox m_bounding_box;
			Point m_mid;
			float m_volume;
		};

	bool overlap(const SphereAggregate& a, const SphereAggregate& b);

	bool overlap(const SphereAggregate& v_a,const Sphere& v_b);
	}

#endif
