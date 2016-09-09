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
#include "solid_deformation.h"

namespace SnowflakeModel
	{
	class VoxelBuilder;

	class Solid
		{
		public:
			static constexpr uint32_t MIRROR_HEADING=1;

			Solid():
				 m_r_max(0)
				,m_flags_dirty(BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY|VOLUME_DIRTY)
				,m_mirror_flags(0)
				{}

			Solid(const DataDump& dump,const char* name);

			VolumeConvex& subvolumeAdd(const VolumeConvex& volume)
				{
				m_subvolumes.push_back(volume);
				m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY
					|VOLUME_DIRTY;
				return m_subvolumes.back();
				}

			VolumeConvex& subvolumeAdd(VolumeConvex&& volume)
				{
				m_subvolumes.push_back(std::move(volume));
				m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY
					|VOLUME_DIRTY;
				return m_subvolumes.back();
				}

			const VolumeConvex* subvolumesBegin() const
				{return m_subvolumes.data();}

			const VolumeConvex* subvolumesEnd() const
				{return m_subvolumes.data()+m_subvolumes.size();}

			VolumeConvex* subvolumesBegin()
				{return m_subvolumes.data();}

			VolumeConvex* subvolumesEnd()
				{return m_subvolumes.data()+m_subvolumes.size();}

			size_t subvolumesCount() const
				{return m_subvolumes.size();}

			const VolumeConvex& subvolumeGet(size_t index) const
				{return m_subvolumes[index];}

			VolumeConvex& subvolumeGet(size_t index)
				{return m_subvolumes[index];}



			void merge(const Matrix& T,const Solid& volume,bool mirrored);

			void merge(const Solid& volume);


			const BoundingBox& boundingBoxGet() const
				{
				if(m_flags_dirty&BOUNDINGBOX_DIRTY)
					{boundingBoxCompute();}
				return m_bounding_box;
				}

			const Point& midpointGet() const
				{
				if(m_flags_dirty&MIDPOINT_DIRTY)
					{midpointCompute();}
				return m_mid;
				}

			float volumeGet() const
				{
				if(m_flags_dirty&VOLUME_DIRTY)
					{volumeCompute();}
				return m_volume;
				}


			float rMaxGet() const
				{
				if(m_flags_dirty&RMAX_DIRTY)
					{rMaxCompute();}
				return m_r_max;
				}

			void geometrySample(VoxelBuilder& builder) const;

			void transform(const Matrix& T,bool mirrored);

			const VolumeConvex* inside(const Point& v) const;

			const VolumeConvex* cross(const VolumeConvex::Face& face) const;

			void centerCentroidAt(const Point& pos_new);
			void centerBoundingBoxAt(const Point& pos_new);
			void normalsFlip();


			void deformationTemplateAdd(SolidDeformation&& deformation)
				{m_deformation_templates.push_back(std::move(deformation));}

			const std::vector<SolidDeformation>& deformationTemplatesGet() const
				{return m_deformation_templates;}

			void mirrorActivate(uint32_t mirror_flags)
				{m_mirror_flags|=mirror_flags;}

			bool mirrorFlagTest(uint32_t mirror_flag) const
				{return m_mirror_flags&mirror_flag;}

			void mirrorDeactivate(uint32_t mirror_flags)
				{m_mirror_flags&=~mirror_flags;}

			size_t facesCount() const
				{
				if(m_flags_dirty&FACES_COUNT_DIRTY)
					{facesCountCompute();}
				return m_n_faces;
				}

			void clear()
				{
				m_subvolumes.clear();
				m_deformation_templates.clear();
				m_flags_dirty=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|FACES_COUNT_DIRTY
					|RMAX_DIRTY|VOLUME_DIRTY;
				}

			void write(const char* id,DataDump& dump) const;


		private:
			static constexpr uint32_t BOUNDINGBOX_DIRTY=1;
			static constexpr uint32_t MIDPOINT_DIRTY=2;
			static constexpr uint32_t FACES_COUNT_DIRTY=4;
			static constexpr uint32_t RMAX_DIRTY=8;
			static constexpr uint32_t VOLUME_DIRTY=16;

			std::vector< VolumeConvex > m_subvolumes;
			std::vector<SolidDeformation> m_deformation_templates;

			mutable BoundingBox m_bounding_box;
			mutable Point m_mid;
			mutable size_t m_n_faces;
			mutable float m_r_max;
			mutable float m_volume;
			mutable uint32_t m_flags_dirty;

			uint32_t m_mirror_flags;


			void midpointCompute() const;
			void boundingBoxCompute() const;
			void facesCountCompute() const;
			void rMaxCompute() const;
			void volumeCompute() const;
		};

	Vector strechToBoundingBox(const Vector& v,const Solid& V);
	Vector strechToSurface(const Vector& v,const Solid& V,float tolerance);
	bool overlap(const Solid& v_a,const Solid& v_b);

	}

#endif
