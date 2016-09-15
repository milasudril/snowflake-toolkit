//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"volume_convex.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "volume_convex.h"
#include "voxel_builder.h"
#include "twins.h"
#include "triangle.h"
#include <stack>

using namespace SnowflakeModel;

constexpr uint16_t VolumeConvex::VERTEX_COUNT;

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<VolumeConvex::Face>::fields[]=
		{
		 {"visible",offsetOf(&VolumeConvex::Face::m_visible),DataDump::MetaObject<bool>().typeGet()}
		,{"v0",offsetOf(&VolumeConvex::Face::m_verts),DataDump::MetaObject<VolumeConvex::VertexIndex>().typeGet()}
		,{"v1",offsetOf(&VolumeConvex::Face::m_verts) + 2,DataDump::MetaObject<VolumeConvex::VertexIndex>().typeGet()}
		,{"v2",offsetOf(&VolumeConvex::Face::m_verts) + 4,DataDump::MetaObject<VolumeConvex::VertexIndex>().typeGet()}
		};
	template<>
	const size_t DataDump::MetaObject<VolumeConvex::Face>::field_count=4;
	}

void VolumeConvex::transform(const Matrix& T)
	{
	auto vertex_current=verticesBegin();
	auto verts_end=verticesEnd();
	while(vertex_current!=verts_end)
		{
		*vertex_current = T * (*vertex_current);
		++vertex_current;
		}

	m_flags_dirty|=MIDPOINT_DIRTY|FACES_NORMAL_DIRTY
		|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY|AREA_VISIBLE_DIRTY;
	}

void VolumeConvex::transformGroup(const std::string& name,const Matrix& T)
	{
	auto i=m_vertex_groups.find(name);
	if(i==m_vertex_groups.end())
		{return;}

	auto vertex_current=i->second.data();
	auto vertex_end=i->second.data() + i->second.size();
	auto verts=m_vertices.begin();
	while(vertex_current!=vertex_end)
		{
		verts[*vertex_current] = T * verts[*vertex_current];
		++vertex_current;
		}
	m_flags_dirty|=MIDPOINT_DIRTY|FACES_NORMAL_DIRTY
		|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY|AREA_VISIBLE_DIRTY;
	}

void VolumeConvex::geometrySample(VoxelBuilder& builder) const
	{
	std::stack<PointInt> nodes;
	nodes.push(builder.quantize(midpointGet()));
	builder.volumeStart(*this);
	while(nodes.size()!=0)
		{
		auto node_current=nodes.top();
		nodes.pop();

		if(builder.fill(node_current))
			{
			nodes.push(node_current+PointInt(-1,0,0,0));
			nodes.push(node_current+PointInt(1 ,0,0,0));
			nodes.push(node_current+PointInt(0,-1,0,0));
			nodes.push(node_current+PointInt(0,1, 0,0));
			nodes.push(node_current+PointInt(0,0,-1,0));
			nodes.push(node_current+PointInt(0,0, 1,0));
			}
		}
	}

void VolumeConvex::facesNormalCompute() const
	{
	auto face_current=facesBegin();
	auto faces_end=facesEnd();
	auto verts=verticesBegin();
	while(face_current!=faces_end)
		{
		auto& v_0=verts[ face_current->vertexGet(0) ];
		auto& v_1=verts[ face_current->vertexGet(1) ];
		auto& v_2=verts[ face_current->vertexGet(2) ];


		Vector u=Vector(v_1 - v_0);
		Vector v=Vector(v_2 - v_0);

		auto N=glm::cross(u,v);
		face_current->m_normal_raw=N;
		face_current->m_normal=glm::normalize(N);

		++face_current;
		}
	m_flags_dirty&=~FACES_NORMAL_DIRTY;
	}

void VolumeConvex::facesMidpointCompute() const
	{
	auto face_current=facesBegin();
	auto faces_end=facesEnd();
	while(face_current!=faces_end)
		{
		Point mid={0,0,0,1};
		size_t k;
		for(k=0;k<VERTEX_COUNT;++k)
			{mid+=face_current->vertexGet(k);}
		mid/=k;
		face_current->m_mid=Point{Vector(mid),1};

		++face_current;
		}
	m_flags_dirty&=~FACES_MIDPOINT_DIRTY;
	}

bool VolumeConvex::inside(const Point& v) const
	{
	if(m_flags_dirty&FACES_MIDPOINT_DIRTY)
		{facesMidpointCompute();}
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}
	auto face_current=facesBegin();
	auto faces_end=facesEnd();
	while(face_current!=faces_end)
		{
		auto x=glm::dot(Vector(v - face_current->m_mid)
			+ 1e-6f*face_current->m_normal,face_current->m_normal);
		if(x > 0 )
			{return 0;}
		++face_current;
		}
	return 1;
	}

void VolumeConvex::midpointCompute() const
	{
	Point mid={0,0,0,0};
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}
	auto face_current=facesBegin();
	auto faces_end=facesEnd();
	while(face_current!=faces_end)
		{
		auto a=Vector(face_current->vertexGet(0));
		auto b=Vector(face_current->vertexGet(1));
		auto c=Vector(face_current->vertexGet(2));
		auto n=face_current->m_normal_raw;

		auto s1=a+b;
		auto s2=b+c;
		auto s3=c+a;

		auto x=n*(s1*s1 + s2*s2 + s3*s3);
		mid+=Point(x,0);

		++face_current;
		}
	mid/=(48*volumeGet());
	mid.w=1;
	m_mid=mid;
	m_flags_dirty&=~MIDPOINT_DIRTY;
	}

void VolumeConvex::volumeCompute() const
	{
	double volume=0;
	m_volume=0;
	auto face_current=facesBegin();
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}

	auto faces_end=facesEnd();
	while(face_current!=faces_end)
		{
		volume+=glm::dot(Vector(face_current->vertexGet(0))
			,face_current->m_normal_raw);
		++face_current;
		}
#ifndef NDEBUG
	if(volume<0)
		{
		face_current=facesBegin();
		while(face_current!=faces_end)
			{
			fprintf(stderr,"%.7g\n",glm::dot(Vector(face_current->vertexGet(0))
				,face_current->m_normal_raw));
			++face_current;
			}
		abort();
		}
#endif
	m_volume=volume/6;
	m_flags_dirty&=~VOLUME_DIRTY;
	}

void VolumeConvex::normalsFlip()
	{
	auto face_current=facesBegin();
	auto faces_end=facesEnd();
	while(face_current!=faces_end)
		{
		face_current->directionChange();
		++face_current;
		}
	m_flags_dirty|=FACES_NORMAL_DIRTY;
	}

void VolumeConvex::areaVisibleCompute() const
	{
	double A=0;
	m_area_visible=0;
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}

	auto i=m_faces_out.data();
	auto end=m_faces_out.data()+m_faces_out.size();
	auto faces=facesBegin();
	while(i!=end)
		{
		A+=glm::length(faces[*i].m_normal_raw);
		++i;
		}
	m_area_visible=A/2;
	m_flags_dirty&=~AREA_VISIBLE_DIRTY;
	}

void VolumeConvex::write(const char* id,DataDump& dump) const
	{
	auto group=dump.groupCreate(id);
	auto group_name=std::string(id);
	dump.write((group_name + "/vertices").c_str(),m_vertices.data()
		,m_vertices.size());
	dump.write((group_name + "/faces").c_str(),m_faces.data()
		,m_faces.size());
	dump.write((group_name + "/faces_out").c_str(),m_faces_out.data()
		,m_faces_out.size());

		{
		auto v_groups_begin=m_vertex_groups.begin();
		auto v_groups_end=m_vertex_groups.end();
		auto group_name_base=group_name + "/vertex_groups";
		auto group_v_groups=dump.groupCreate(group_name_base.c_str());
		group_name_base+='/';
		while(v_groups_begin!=v_groups_end)
			{
			auto group_name_current=group_name_base + v_groups_begin->first;
			dump.write(group_name_current.c_str()
				,v_groups_begin->second.data(),v_groups_begin->second.size());
			++v_groups_begin;
			}
		}
	}

VolumeConvex::VolumeConvex(const DataDump& dump,const char* id)
	{
	auto group=dump.groupOpen(id);
	auto group_name=std::string(id);
	m_vertices=dump.arrayGet<Vertex>((group_name + "/vertices").c_str());
	m_faces=dump.arrayGet<Face>((group_name + "/faces").c_str());
	m_faces_out=dump.arrayGet<FaceIndex>((group_name + "/faces_out").c_str());

	m_flags_dirty=MIDPOINT_DIRTY|FACES_NORMAL_DIRTY|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY
		|AREA_VISIBLE_DIRTY;

		{
		auto v_group_name_base=group_name + "/vertex_groups";
		auto v_group=dump.groupOpen(v_group_name_base.c_str());
		v_group_name_base+='/';
		dump.iterate(*v_group,[&v_group_name_base,this,&dump](const char* name)
			{
			auto group_name_current=v_group_name_base + name;
			m_vertex_groups[name]=dump.arrayGet<VertexIndex>(group_name_current.c_str());
			});
		}
	}

bool SnowflakeModel::overlap(const VolumeConvex& a,const VolumeConvex& b)
	{
//	TODO: check BBs first so we can accept early

	auto face_a=a.facesBegin();
	auto faces_a_end=a.facesEnd();
	auto v_a=a.verticesBegin();
	auto v_b=b.verticesBegin();

	auto face_b_0=b.facesBegin();
	auto faces_b_end=b.facesEnd();

//	FIXME: Add dirty flags to face? And use Face::normalGet()?
//	Move dirty test into facesNormalCompute?
//	Or not??
	if(a.m_flags_dirty&VolumeConvex::FACES_NORMAL_DIRTY)
		{a.facesNormalCompute();}

	if(b.m_flags_dirty&VolumeConvex::FACES_NORMAL_DIRTY)
		{b.facesNormalCompute();}

	while(face_a!=faces_a_end)
		{
		auto T_1=Triangle
			{ 
				{
				 v_a[face_a->vertexGet(0)]
				,v_a[face_a->vertexGet(1)]
				,v_a[face_a->vertexGet(2)]
				}
			,face_a->m_normal
			};
		auto face_b=face_b_0;
		while(face_b!=faces_b_end)
			{
			auto T_2=Triangle
				{ 
					{
					 v_b[face_b->vertexGet(0)]
					,v_b[face_b->vertexGet(1)]
					,v_b[face_b->vertexGet(2)]
					}
				,face_b->m_normal
				};
			if(overlap(T_1,T_2))
				{return 1;}
			++face_b;
			}
		++face_a;
		}

	return 0;
	}
