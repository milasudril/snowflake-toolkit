//@	{
//@	 "targets":[{"name":"sphere_aggregate.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"sphere_aggregate.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_SPHEREAGGREGATE_H
#define SNOWFLAKEMODEL_SPHEREAGGREGATE_H

#include "sphere.h"
#include "twins.h"

namespace SnowflakeModel
	{
	class Grid;
	class DataDump;
	class SphereAggregate
		{
		public:
			SphereAggregate():m_bounding_box{{INFINITY,INFINITY,INFINITY,1.0f}
				,{-INFINITY,-INFINITY,-INFINITY,1.0f}},m_mid{0.0f,0.0f,0.0f,1.0f}
				,m_extrema{{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f,1.0f}}
				,m_volume(0.0f)
				{}


			explicit SphereAggregate(const DataDump& dump,const char* name);

			Sphere& subvolumeAdd(const Sphere& volume,float overlap)
				{
				auto e=extremaNew(volume);
				m_subvolumes.push_back(volume);
				auto V=static_cast<double>( volume.volumeGet() );
				m_volume+=V - static_cast<double>(overlap);
				auto bb=volume.boundingBoxGet();
				m_bounding_box.m_min=glm::min(m_bounding_box.m_min
					,bb.m_min);
				m_bounding_box.m_max=glm::max(m_bounding_box.m_max
					,bb.m_max);
				m_mid+=volume.midpointGet()*volume.volumeGet();
				m_extrema=e;
				return m_subvolumes.back();
				}

			Sphere& subvolumeAddNoDMax(const Sphere& volume,float overlap)
				{
				m_subvolumes.push_back(volume);
				auto V=static_cast<double>( volume.volumeGet() );
				m_volume+=V - static_cast<double>(overlap);
				auto bb=volume.boundingBoxGet();
				m_bounding_box.m_min=glm::min(m_bounding_box.m_min
					,bb.m_min);
				m_bounding_box.m_max=glm::max(m_bounding_box.m_max
					,bb.m_max);
				m_mid+=volume.midpointGet()*volume.volumeGet();
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
				{return m_mid/static_cast<float>(m_volume);}

			float volumeGet() const noexcept
				{return m_volume;}

			std::pair<Point,Vector> shoot(const Point& source,const Vector& direction
				,float E_0,float decay_distance) const noexcept;

			void geometrySample(Grid& grid) const;

			Twins<Point> extremaGet() const noexcept
				{return m_extrema;}

			void write(const char* id,DataDump& dump) const;

		private:
			std::vector<Sphere> m_subvolumes;
			BoundingBox m_bounding_box;
			Point m_mid;
			Twins<Point> m_extrema;
			double m_volume;

			Twins<Point> extremaNew(const Sphere& volume) const noexcept;
		};

	bool overlap(const SphereAggregate& a, const SphereAggregate& b);

	bool overlap(const SphereAggregate& v_a,const Sphere& v_b);

	size_t overlap(const SphereAggregate& v_a,const SphereAggregate& v_b
		,size_t subvols,double& overlap_res) noexcept;

	size_t overlap(const SphereAggregate& v_a,const Sphere& v_b,size_t subvols
		,double& overlap_res) noexcept;
	}

#endif
