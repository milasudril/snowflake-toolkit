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
#include "twins.h"
#include "triangle.h"
#include "grid.h"
#include "sphere.h"
#include <stack>

using namespace SnowflakeModel;

constexpr uint16_t VolumeConvex::VERTEX_COUNT;

namespace SnowflakeModel
	{
	template<>
	const DataDump::FieldDescriptor DataDump::MetaObject<VolumeConvex::Face>::fields[]=
		{
		 {"visible",offsetOf(&VolumeConvex::Face::m_visible),DataDump::MetaObject<decltype(VolumeConvex::Face::m_visible)>().typeGet()}
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
		|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY|AREA_VISIBLE_DIRTY
		|BOUNDING_BOX_DIRTY;
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
		|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY|AREA_VISIBLE_DIRTY|BOUNDING_BOX_DIRTY;
	}


static PointInt seedGenerate(const VolumeConvex& v,const Grid& grid)
	{
//	Try some points inside the bounding box to see if we get a hit.
	PointInt seed;
	std::minstd_rand rng;
	auto& bb=v.boundingBoxGet();
	
 	auto size=grid.quantize(bb.m_max) - grid.quantize(bb.m_min);
	if(size.x<2)
		{throw "Too few raster points in x direction";}
		
	if(size.y<2)
		{throw "Too few raster points in y direction";}

	if(size.z<2)
		{throw "Too few raster points in z direction";}
	
	do
		{
		seed=grid.quantize( randomPoint(bb,rng) );
		}
	while(!v.inside( grid.dequantize(seed)) );
	return seed;
	}

void VolumeConvex::geometrySample(Grid& grid) const
	{
	std::stack<PointInt> nodes;

	nodes.push(seedGenerate(*this,grid));
	auto& bb=boundingBoxGet();
	grid.bitAnd(grid.quantize(bb.m_min),grid.quantize(bb.m_max),0xf0);
	while(nodes.size()!=0)
		{
		auto node_current=nodes.top();
		nodes.pop();
		auto offset=grid.offsetGet(node_current);
		if(!grid.cellFilled(offset,0xf))
			{
			if(inside(grid.dequantize(node_current) ) )
				{
				grid.cellFill(offset,0xff);
				nodes.push(node_current+PointInt(-1,0,0,0));
				nodes.push(node_current+PointInt(1 ,0,0,0));
				nodes.push(node_current+PointInt(0,-1,0,0));
				nodes.push(node_current+PointInt(0,1, 0,0));
				nodes.push(node_current+PointInt(0,0,-1,0));
				nodes.push(node_current+PointInt(0,0, 1,0));
				}
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

static Vector hittestDirection(const Point& v, const Point& mid
	,const BoundingBox& bb)
	{
//	Shoot towards center, or outwards (depending on v) to reduce
//	the risk of numerical errors.

	auto delta=Vector(mid - v);
	auto n=glm::length(delta);
	if(n>.125f*glm::length(Vector(bb.m_max - bb.m_min)))
		{return delta/n;}

	return Vector(1.0f,0.0f,0.0f);
	}

bool VolumeConvex::inside(const Point& v) const
	{
//	Check bounding box first
	if(!::inside(v,boundingBoxGet()))
		{return 0;}

	auto dir=hittestDirection(v,midpointGet(),boundingBoxGet());
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}
	auto face_current=facesBegin();
	auto faces_end=facesEnd();
	auto verts=verticesBegin();
	size_t intersect_count=0;
	float intersection;
	while(face_current!=faces_end)
		{
		auto T=Triangle
			{
				{
				 verts[face_current->vertexGet(0)]
				,verts[face_current->vertexGet(1)]
				,verts[face_current->vertexGet(2)]
				}
			,face_current->m_normal
			};
		if(intersects(T,v,dir,intersection))
			{++intersect_count;}
		++face_current;
		}
	return intersect_count%2==1;
	}

void VolumeConvex::midpointCompute() const
	{
	Point mid={0,0,0,0};
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}
	auto face_current=facesBegin();
	auto faces_end=facesEnd();
	auto verts=verticesBegin();
	while(face_current!=faces_end)
		{
		auto a=Vector(verts[ face_current->vertexGet(0) ]);
		auto b=Vector(verts[ face_current->vertexGet(1) ]);
		auto c=Vector(verts[ face_current->vertexGet(2) ]);
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
	m_volume=0;
	double volume=0;
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}

	auto face_current=facesBegin();
	auto faces_end=facesEnd();
	auto verts=verticesBegin();
	while(face_current!=faces_end)
		{
		volume+=glm::dot(Vector(verts[ face_current->vertexGet(0) ])
			,face_current->m_normal_raw);
		++face_current;
		}
#ifndef NDEBUG
	if(volume<0)
		{
		face_current=facesBegin();
		while(face_current!=faces_end)
			{
			fprintf(stderr,"%.7g\n",glm::dot(Vector(verts[ face_current->vertexGet(0) ])
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
		|AREA_VISIBLE_DIRTY|BOUNDING_BOX_DIRTY;

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
	if(!overlap(a.boundingBoxGet(),b.boundingBoxGet()))
		{return 0;}

	auto face_a=a.facesBegin();
	auto faces_a_end=a.facesEnd();
	auto v_a=a.verticesBegin();
	auto v_b=b.verticesBegin();

	auto face_b_0=b.facesBegin();
	auto faces_b_end=b.facesEnd();

	if(a.normalsDirty())
		{a.facesNormalCompute();}

	if(b.normalsDirty())
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

void VolumeConvex::boundingBoxCompute() const noexcept
	{
	BoundingBox bb{{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f,1.0f}};
	auto verts_begin=verticesBegin();
	auto verts_end=verticesEnd();
	if(verts_begin==verts_end)
		{
		m_bounding_box=bb;
		m_flags_dirty&=~BOUNDING_BOX_DIRTY;
		return;
		}
	bb.m_min=*verts_begin;
	bb.m_max=bb.m_min;
	++verts_begin;
	while(verts_begin!=verts_end)
		{
		bb.m_min=glm::min(bb.m_min,*verts_begin);
		bb.m_max=glm::max(bb.m_max,*verts_begin);
		++verts_begin;
		}
	m_bounding_box=bb;
	m_flags_dirty&=~BOUNDING_BOX_DIRTY;
	}

std::pair<Triangle,float> VolumeConvex::shoot(const Point& source,const Vector& direction
	,float E_0
	,float decay_distance
	,bool backface_culling) const noexcept
	{
	std::pair<Triangle,float> ret{Triangle{},INFINITY};
	auto faces_begin=facesBegin();
	auto faces_end=facesEnd();
	auto verts=verticesBegin();

	if(normalsDirty())
		{facesNormalCompute();}

	while(faces_begin!=faces_end)
		{
		std::pair<Triangle,float> temp
			{
			Triangle
				{
					{
					 verts[faces_begin->vertexGet(0)]
					,verts[faces_begin->vertexGet(1)]
					,verts[faces_begin->vertexGet(2)]
					}
				,faces_begin->m_normal
				}
			,0.0f
			};
		float out;
		if(intersects(temp.first,source,direction,out))
			{
			auto proj=glm::dot(temp.first.m_normal,direction);
			if(proj>=0 || !backface_culling) //Culling
				{
				auto mid=(temp.first.vertexGet(0) + temp.first.vertexGet(1) + temp.first.vertexGet(2))/3.0f;
				temp.second=glm::distance(mid,source);
				auto E_out=E_0*(1.0f - proj)
					*std::exp(-temp.second/decay_distance);
				if(temp.second < ret.second && E_out<1.0f)
					{ret=temp;}
				}
			}

		++faces_begin;
		}

	return ret;
	}



static void vertexAddNormalized(VolumeConvex& v,const Vector& vec)
	{
	v.vertexAdd(VolumeConvex::Vertex(vec/glm::length(vec),1.0f));
	}

static uint32_t makeEdgeKey(uint16_t a,uint16_t b)
	{
	if(a<b)
		{return (a<<16)|b;}
	return (b<<16)|a;
	}

static uint16_t vertexGenerate(std::map<uint32_t,uint16_t>& vert_indices
	,VolumeConvex& v
	,uint16_t vi1,uint16_t vi2)
	{
	auto key=makeEdgeKey(vi1,vi2);
	auto i=vert_indices.find(key);
	if(i!=vert_indices.end())
		{return i->second;}

	auto v1=v.vertexGet(vi1);
	auto v2=v.vertexGet(vi2);

	auto mid=0.5f*(v1 + v2);
	auto index=v.verticesCount();
	vertexAddNormalized(v,Vector(mid));
	vert_indices.insert({key,index});
	return index;
	}


VolumeConvex::VolumeConvex(const Sphere& sphere,unsigned int subdivs):
m_flags_dirty(MIDPOINT_DIRTY|FACES_NORMAL_DIRTY|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY
	|AREA_VISIBLE_DIRTY)
	{
//	From http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

	auto constexpr t = (1.0f + std::sqrt(5.0f)) / 2.0f;

	vertexAddNormalized(*this,Vector(-1.0f,  t,  0.0f));
	vertexAddNormalized(*this,Vector( 1.0f,  t,  0.0f));
	vertexAddNormalized(*this,Vector(-1.0f, -t,  0.0f));
	vertexAddNormalized(*this,Vector( 1.0f, -t,  0.0f));

	vertexAddNormalized(*this,Vector( 0.0f, -1.0f,  t));
	vertexAddNormalized(*this,Vector( 0.0f,  1.0f,  t));
	vertexAddNormalized(*this,Vector( 0.0f, -1.0f, -t));
	vertexAddNormalized(*this,Vector( 0.0f,  1.0f, -t));

	vertexAddNormalized(*this,Vector( t,  0.0f, -1.0f));
	vertexAddNormalized(*this,Vector( t,  0.0f,  1.0f));
	vertexAddNormalized(*this,Vector(-t,  0.0f, -1.0f));
	vertexAddNormalized(*this,Vector(-t,  0.0f,  1.0f));

	std::vector<Face> faces;

	faces.push_back(Face(0, 11, 5));
	faces.push_back(Face(0, 5, 1));
	faces.push_back(Face(0, 1, 7));
	faces.push_back(Face(0, 7, 10));
	faces.push_back(Face(0, 10, 11));

	faces.push_back(Face(1, 5, 9));
	faces.push_back(Face(5, 11, 4));
	faces.push_back(Face(11, 10, 2));
	faces.push_back(Face(10, 7, 6));
	faces.push_back(Face(7, 1, 8));

	faces.push_back(Face(3, 9, 4));
	faces.push_back(Face(3, 4, 2));
	faces.push_back(Face(3, 2, 6));
	faces.push_back(Face(3, 6, 8));
	faces.push_back(Face(3, 8, 9));

	faces.push_back(Face(4, 9, 5));
	faces.push_back(Face(2, 4, 11));
	faces.push_back(Face(6, 2, 10));
	faces.push_back(Face(8, 6, 7));
	faces.push_back(Face(9, 8, 1));

//	Subdivide
	std::map<uint32_t,uint16_t> midpoints; //Midpoint cache
	for(unsigned int k=0;k<subdivs;++k)
		{
		std::vector<Face> faces_temp;
		auto ptr=faces.data();
		auto ptr_end=ptr + faces.size();
		while(ptr!=ptr_end)
			{
			auto v_0=ptr->vertexGet(0);
			auto v_1=ptr->vertexGet(1);
			auto v_2=ptr->vertexGet(2);

			auto v_a=vertexGenerate(midpoints,*this,v_0,v_1);
			auto v_b=vertexGenerate(midpoints,*this,v_1,v_2);
			auto v_c=vertexGenerate(midpoints,*this,v_2,v_0);

			faces_temp.push_back(Face(v_0,v_a,v_c));
			faces_temp.push_back(Face(v_1,v_b,v_a));
			faces_temp.push_back(Face(v_2,v_c,v_b));
			faces_temp.push_back(Face(v_a,v_b,v_c));
			++ptr;
			}
		faces=std::move(faces_temp);
		}

	m_faces=std::move(faces);

	////make all faces visible
		{
		auto ptr=facesBegin();
		auto ptr_end=facesEnd();
		while(ptr!=ptr_end)
			{
			faceOutAdd((ptr_end - ptr) - 1);
			++ptr;
			}
		}

	////Apply transformation
		{
		auto ptr=verticesBegin();
		auto ptr_end=verticesEnd();
		Matrix T;
		auto r=sphere.radiusGet();
		auto V_0=4.0f*std::acos(-1)/3.0f;
		r*=std::pow(V_0/volumeGet(),1.0f/3.0f);

		T=glm::translate(T,Vector(sphere.midpointGet()));
		T=glm::scale(T,Vector(r,r,r));
		while(ptr!=ptr_end)
			{
			*ptr=T*(*ptr);
			++ptr;
			}
		}
	}
