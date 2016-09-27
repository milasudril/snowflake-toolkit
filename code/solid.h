//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"solid.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_SOLID_H
#define SNOWFLAKEMODEL_SOLID_H

#include "volume_convex.h"
#include "bounding_box.h"
#include "solid_deformation.h"
#include "twins.h"

namespace SnowflakeModel
	{
	class VoxelBuilder;

	class Solid
		{
		public:
			static constexpr uint32_t MIRROR_HEADING=1;

			Solid() noexcept:
				 m_n_faces_tot(0)
				,m_r_max(0)
				,m_volume(0)
				,m_extrema{{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f,1.0f}}
				,m_flags_dirty(DMAX_DIRTY|BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY)
				,m_overlap_count(0)
				,m_mirror_flags(0)
				{}

			Solid(const DataDump& dump,const char* name);

			VolumeConvex& subvolumeAdd(const VolumeConvex& volume
				,double overlap_est,size_t overlap_count)
				{
			//	If this fails, the object is in a bad state hmm
				extremaUpdate(volume);
				boundingBoxUpdate(volume);
				m_subvolumes.push_back(volume);
				m_flags_dirty|=MIDPOINT_DIRTY|RMAX_DIRTY;
				m_n_faces_tot+=volume.facesCount();
				m_volume+=volume.volumeGet()-overlap_est;
				m_overlap_count+=overlap_count;
				return m_subvolumes.back();
				}

			VolumeConvex& subvolumeAdd(VolumeConvex&& volume,double overlap_est
				,size_t overlap_count)
				{
				m_n_faces_tot+=volume.facesCount();
				m_volume+=volume.volumeGet() - overlap_est;
				extremaUpdate(volume);
				boundingBoxUpdate(volume);

			//	If this fails, the object is in a bad state hmm
				m_subvolumes.push_back(std::move(volume));
				m_flags_dirty|=MIDPOINT_DIRTY|RMAX_DIRTY;
				m_overlap_count+=overlap_count;
				return m_subvolumes.back();
				}

			const VolumeConvex* subvolumesBegin() const noexcept
				{return m_subvolumes.data();}

			const VolumeConvex* subvolumesEnd() const noexcept
				{return m_subvolumes.data()+m_subvolumes.size();}

			VolumeConvex* subvolumesBegin() noexcept
				{return m_subvolumes.data();}

			VolumeConvex* subvolumesEnd() noexcept
				{return m_subvolumes.data()+m_subvolumes.size();}

			size_t subvolumesCount() const noexcept
				{return m_subvolumes.size();}

			const VolumeConvex& subvolumeGet(size_t index) const noexcept
				{return m_subvolumes[index];}

			VolumeConvex& subvolumeGet(size_t index) noexcept
				{return m_subvolumes[index];}

			size_t overlapCount() const noexcept
				{return m_overlap_count;}



			void merge(const Matrix& T,const Solid& volume,bool mirrored);

			void merge(const Solid& volume,double overlap_est,size_t overlap_count);


			const BoundingBox& boundingBoxGet() const noexcept
				{
				if(m_flags_dirty&BOUNDINGBOX_DIRTY)
					{boundingBoxCompute();}
				return m_bounding_box;
				}

			const Point& midpointGet() const noexcept
				{
				if(m_flags_dirty&MIDPOINT_DIRTY)
					{midpointCompute();}
				return m_mid;
				}

			float volumeGet() const noexcept
				{
				if(m_flags_dirty&VOLUME_DIRTY || m_volume<1e-7)
					{volumeCompute();}
				return m_volume;
				}


			float rMaxGet() const noexcept
				{
				if(m_flags_dirty&RMAX_DIRTY)
					{rMaxCompute();}
				return m_r_max;
				}

			Twins<glm::vec4> extremaGet() const noexcept
				{
				if(m_flags_dirty&DMAX_DIRTY)
					{dMaxCompute();}
				return m_extrema;
				}

			void geometrySample(VoxelBuilder& builder) const;

			void transform(const Matrix& T,bool mirrored) noexcept;

			const VolumeConvex* inside(const Point& v) const noexcept;

			const VolumeConvex* cross(const VolumeConvex::Face& face) const noexcept;

			size_t cross(const VolumeConvex::Face& face,size_t count_max) const noexcept;

			void centerCentroidAt(const Point& pos_new) noexcept;
			void centerBoundingBoxAt(const Point& pos_new) noexcept;
			void normalsFlip() noexcept;


			void deformationTemplateAdd(SolidDeformation&& deformation)
				{m_deformation_templates.push_back(std::move(deformation));}

			const std::vector<SolidDeformation>& deformationTemplatesGet() const noexcept
				{return m_deformation_templates;}

			void mirrorActivate(uint32_t mirror_flags) noexcept
				{m_mirror_flags|=mirror_flags;}

			bool mirrorFlagTest(uint32_t mirror_flag) const noexcept
				{return m_mirror_flags&mirror_flag;}

			void mirrorDeactivate(uint32_t mirror_flags) noexcept
				{m_mirror_flags&=~mirror_flags;}

			size_t facesCount() const noexcept
				{
				assert(m_n_faces_tot!=0);
				return m_n_faces_tot;
				}

			void clear() noexcept
				{
				m_n_faces_tot=0;
				m_subvolumes.clear();
				m_deformation_templates.clear();
				m_volume=0;
				m_r_max=0;
				m_extrema={{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f,1.0f}};
				m_bounding_box={{INFINITY,INFINITY,INFINITY,1.0f},{-INFINITY,-INFINITY,-INFINITY,1.0f}};
				}

			void write(const char* id,DataDump& dump) const;

			std::pair<Triangle,float> shoot(const Point& source,const Vector& direction
				,float E_0,float decay_distance
				,bool backface_culling) const noexcept;


		private:
			static constexpr uint32_t BOUNDINGBOX_DIRTY=1;
			static constexpr uint32_t MIDPOINT_DIRTY=2;
			static constexpr uint32_t RMAX_DIRTY=8;
			static constexpr uint32_t VOLUME_DIRTY=16;
			static constexpr uint32_t DMAX_DIRTY=4;

			std::vector< VolumeConvex > m_subvolumes;
			size_t m_n_faces_tot;
			std::vector<SolidDeformation> m_deformation_templates;

			mutable BoundingBox m_bounding_box;
			mutable Point m_mid;
			mutable float m_r_max;
			mutable float m_volume;
			mutable Twins<glm::vec4> m_extrema;
			mutable uint32_t m_flags_dirty;


			size_t m_overlap_count;
			uint32_t m_mirror_flags;

			void midpointCompute() const noexcept;
			void boundingBoxCompute() const noexcept;
			void rMaxCompute() const noexcept;
			void dMaxCompute() const noexcept;
			void volumeCompute() const noexcept;

			void extremaUpdate(const VolumeConvex& v) const noexcept;
			void extremaUpdate(const Solid& s) const noexcept;
			void boundingBoxUpdate(const VolumeConvex& v) const noexcept;
			void boundingBoxUpdate(const Solid& v) const noexcept;
		};

	Vector strechToBoundingBox(const Vector& v,const Solid& V) noexcept;
	Vector strechToSurface(const Vector& v,const Solid& V,float tolerance) noexcept;
	bool overlap(const Solid& v_a,const Solid& v_b) noexcept;
	size_t overlap(const Solid& v_a,const Solid& v_b,size_t subvols
		,double& overlap_res) noexcept;

	}

#endif
