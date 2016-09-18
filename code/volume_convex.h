//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"volume_convex.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"volume_convex.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_VOLUMECONVEX_H
#define SNOWFLAKEMODEL_VOLUMECONVEX_H

#include "vector.h"
#include "triangle.h"
#include "bounding_box.h"
#include <vector>
#include <map>

namespace SnowflakeModel
	{
	class VoxelBuilder;
	class VolumeConvex
		{
		public:
 			typedef uint16_t VertexIndex;
			typedef uint16_t FaceIndex;
			static constexpr uint16_t VERTEX_COUNT=3;
			typedef Point Vertex;
			class Face
				{
				public:
					Face(){}

					Face(VertexIndex v0,VertexIndex v1,VertexIndex v2):m_visible(0)
						{
						m_verts[0]=v0;
						m_verts[1]=v1;
						m_verts[2]=v2;
						}

					mutable Vector m_normal;
					mutable Vector m_normal_raw;
					uint16_t m_visible;

					void directionChange() noexcept
						{std::swap(m_verts[0],m_verts[1]);}

					VertexIndex vertexGet(int index) const noexcept
						{return m_verts[index];}

				private:
					VertexIndex m_verts[VERTEX_COUNT];
					friend class DataDump::MetaObject<Face>;
				};

			VolumeConvex():
				m_flags_dirty(MIDPOINT_DIRTY
					|FACES_NORMAL_DIRTY|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY
					|AREA_VISIBLE_DIRTY)
				{}

			VolumeConvex(const DataDump& dump,const char* id);

			VolumeConvex(const VolumeConvex& vc)=default;

			size_t vertexAdd(const Matrix& T,const Point& v)
				{
				m_vertices.push_back( T*v );
				return m_vertices.size();
				}

			size_t vertexAdd(const Vertex& v)
				{
				m_vertices.push_back(v);
				return m_vertices.size();
				}

			const Vertex* verticesBegin() const
				{return m_vertices.data();}

			const Vertex* verticesEnd() const
				{return m_vertices.data()+m_vertices.size();}

			Vertex* verticesBegin()
				{return m_vertices.data();}

			Vertex* verticesEnd()
				{return m_vertices.data()+m_vertices.size();}

			size_t verticesCount() const
				{return m_vertices.size();}

			const Vertex& vertexGet(size_t index) const
				{return m_vertices[index];}



			void faceAdd(const Face& face)
				{m_faces.push_back(face);}

			const Face* facesBegin() const
				{return m_faces.data();}

			const Face* facesEnd() const
				{return m_faces.data()+m_faces.size();}

			Face* facesBegin()
				{return m_faces.data();}

			Face* facesEnd()
				{return m_faces.data()+m_faces.size();}


			const Face& faceGet(FaceIndex i) const
				{return m_faces[i];}

			size_t facesCount() const
				{return m_faces.size();}

			void vertexGroupSet(const std::string& name,VertexIndex index)
				{m_vertex_groups[name].push_back(index);}

			Triangle triangleGet(FaceIndex i) const noexcept
				{
				if(m_flags_dirty&FACES_NORMAL_DIRTY)
					{facesNormalCompute();}
				auto verts=verticesBegin();
				auto face=facesBegin() + i;
				return Triangle
					{ 
						{
						 verts[face->vertexGet(0)]
						,verts[face->vertexGet(1)]
						,verts[face->vertexGet(2)]
						}
					,face->m_normal
					};
				}

			Triangle triangleOutGet(FaceIndex face_index_out) const noexcept
				{
				return triangleGet(m_faces_out[face_index_out]);
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

			float areaVisibleGet() const
				{
				if(m_flags_dirty&AREA_VISIBLE_DIRTY)
					{areaVisibleCompute();}
				return m_area_visible;
				}


			void transform(const Matrix& T);
			void transformGroup(const std::string& name,const Matrix& T);

			bool inside(const Point& v) const;

			void geometrySample(VoxelBuilder& builder) const;

			void normalsFlip();

			void facesNormalCompute() const;
			void midpointCompute() const;
			void volumeCompute() const;
			void areaVisibleCompute() const;
			void boundingBoxCompute() const noexcept;

			FaceIndex facesOutCount() const noexcept
				{return m_faces_out.size();}

			const FaceIndex* facesOutBegin() const noexcept 
				{return m_faces_out.data();}

			const FaceIndex* facesOutEnd() const
				{return facesOutBegin()+facesOutCount();}

			void faceOutAdd(FaceIndex i)
				{
				m_faces_out.push_back(i);
				m_faces[i].m_visible=1;
				assert(m_flags_dirty&AREA_VISIBLE_DIRTY);
				}

			void faceOutRemove(size_t i)
				{
				assert(i<m_faces_out.size());
				faceOutGet(i).m_visible=0;
				m_faces_out.erase(m_faces_out.begin()+i);
				m_flags_dirty|=AREA_VISIBLE_DIRTY;
				}

			const Face& faceOutGet(size_t i) const noexcept
				{return m_faces[m_faces_out[i]];}

			Face& faceOutGet(size_t i) noexcept
				{return m_faces[m_faces_out[i]];}

			bool normalsDirty() const noexcept
				{return m_flags_dirty&FACES_NORMAL_DIRTY;}

			bool boundingBoxDirty() const noexcept
				{return m_flags_dirty&BOUNDING_BOX_DIRTY;}

			const BoundingBox& boundingBoxGet() const noexcept
				{
				if(boundingBoxDirty())
					{boundingBoxCompute();}
				return m_bounding_box;
				}

			void write(const char* id,DataDump& dump) const;


		private:
			std::vector< Vertex > m_vertices;
			std::vector< Face > m_faces;
			std::vector<FaceIndex> m_faces_out;
			std::map< std::string, std::vector<VertexIndex> > m_vertex_groups;

			mutable Point m_mid;
			mutable BoundingBox m_bounding_box;
			mutable float m_volume;
			mutable float m_area_visible;
			mutable uint32_t m_flags_dirty;

			static constexpr uint32_t MIDPOINT_DIRTY=1;
			static constexpr uint32_t FACES_NORMAL_DIRTY=2;
			static constexpr uint32_t FACES_MIDPOINT_DIRTY=4;
			static constexpr uint32_t VOLUME_DIRTY=8;
			static constexpr uint32_t AREA_VISIBLE_DIRTY=16;
			static constexpr uint32_t BOUNDING_BOX_DIRTY=32;
		};

	bool overlap(const VolumeConvex& a,const VolumeConvex& b);
	}

#endif
