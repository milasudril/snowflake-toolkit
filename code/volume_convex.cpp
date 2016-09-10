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

VolumeConvex::VolumeConvex(const VolumeConvex& vc):
	 m_vertices(vc.m_vertices)
	,m_faces(vc.m_faces)
	,m_faces_out(vc.m_faces_out)
	,m_vertex_groups(vc.m_vertex_groups)
	,m_bounding_box(vc.m_bounding_box)
	,m_mid(vc.m_mid)
	,m_volume(vc.m_volume)
	,m_area_visible(vc.m_area_visible)
	,m_flags_dirty(vc.m_flags_dirty)
	{
	auto face_current=facesBegin();
	while(face_current!=facesEnd())
		{
		face_current->parentSet(*this);
		++face_current;
		}
	}

void VolumeConvex::transform(const Matrix& T)
	{
	auto vertex_current=verticesBegin();
	while(vertex_current!=verticesEnd())
		{
		*vertex_current = T * (*vertex_current);
		++vertex_current;
		}

	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|FACES_NORMAL_DIRTY
		|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY|AREA_VISIBLE_DIRTY;
	}

void VolumeConvex::transformGroup(const std::string& name,const Matrix& T)
	{
	auto i=m_vertex_groups.find(name);
	if(i==m_vertex_groups.end())
		{return;}

	auto vertex_current=i->second.data();
	auto vertex_end=i->second.data() + i->second.size();
	while(vertex_current!=vertex_end)
		{
		m_vertices[*vertex_current] = T * m_vertices[*vertex_current];
		++vertex_current;
		}
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|FACES_NORMAL_DIRTY
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
	while(face_current!=facesEnd())
		{
		auto& v_0=face_current->vertexGet(0);
		auto& v_1=face_current->vertexGet(1);
		auto& v_2=face_current->vertexGet(2);


		Vector u=Vector(v_1 - v_0);
		Vector v=Vector(v_2 - v_0);

		face_current->m_normal_raw=glm::cross(u,v);
		face_current->m_normal=glm::normalize(face_current->m_normal_raw);

		++face_current;
		}
	m_flags_dirty&=~FACES_NORMAL_DIRTY;
	}

void VolumeConvex::facesMidpointCompute() const
	{
	auto face_current=facesBegin();
	while(face_current!=facesEnd())
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
	while(face_current!=facesEnd())
		{
		auto x=glm::dot(Vector(v - face_current->m_mid)
			+ 1e-6f*face_current->m_normal,face_current->m_normal);
		if(x > 0 )
			{return 0;}
		++face_current;
		}
	return 1;
	}

namespace
	{
	float Pi(const Vector& plane_normal,const VolumeConvex::Vertex& V_0
		,const Point& P)
		{
		return glm::dot(plane_normal,Vector(P-V_0));
		}

	template<class T>
	struct Tripple
		{
		T objs[3];
		T& operator[](unsigned int index)
			{return objs[index];}
		const T& operator[](unsigned int index) const
			{return objs[index];}
		};

	template<class T>
	Tripple<int> i_other(const Tripple<T>& Pi)
		{
		if(Pi[0]*Pi[1]>0)
			{return Tripple<int>{1,2,0};}
		if(Pi[0]*Pi[2]>0)
			{return Tripple<int>{0,1,2};}
		return Tripple<int>{2,0,1};
		}

	template<class T>
	Twins<T> sort(const Twins<T>& t)
		{
		return t.first<t.second? t : Twins<T>{t.second,t.first};
		}

	bool overlap(const Twins<float> a,const Twins<float>& b)
		{
		if(a.second<=b.first+1e-6f || b.second<=a.first+1e-6f)
			{return 0;}
		return 1;
		}



	bool triangle_intersect(const VolumeConvex::Face& T_1,const VolumeConvex::Face& T_2)
		{
		assert(&T_1!=&T_2);
		auto d=glm::cross(T_1.m_normal,T_2.m_normal);

	//	T_1
		Tripple<float> pi;
		Tripple<float> t;
		for(unsigned int k=0;k<VolumeConvex::VERTEX_COUNT;++k)
			{
			pi[k]=Pi(T_2.m_normal,T_2.vertexGet(0),T_1.vertexGet(k));
		//	Robustness check
			pi[k]=fabs(pi[k])>1.e-6f? pi[k]: 0.0f;
			}

		//	Triangles are coplanar or does not intersect
		if(pi[0]*pi[1]>=0.0f && pi[0]*pi[2]>=0.0f)
			{return 0;}

		auto i_ref=i_other(pi);
		for(unsigned int k=0;k<VolumeConvex::VERTEX_COUNT;++k)
			{t[k]=glm::dot(d,Vector( T_1.vertexGet(i_ref[k])) );}

		auto t_1=sort(Twins<float>
			{
			 (t[0]*pi[i_ref[1]] - t[1]*pi[i_ref[0]])/(pi[i_ref[1]] - pi[i_ref[0]])
			,(t[1]*pi[i_ref[2]] - t[2]*pi[i_ref[1]])/(pi[i_ref[2]] - pi[i_ref[1]])
			});

	//	T_2
		for(unsigned int k=0;k<VolumeConvex::VERTEX_COUNT;++k)
			{
			pi[k]=Pi(T_1.m_normal,T_1.vertexGet(0),T_2.vertexGet(k));
		//	Robustness check
			pi[k]=fabs(pi[k])>1.e-6f? pi[k]: 0.0f;
			}

		//	Triangles are coplanar or does not intersect
		if(pi[0]*pi[1]>=0.0f && pi[0]*pi[2]>=0.0f)
			{return 0;}

		i_ref=i_other(pi);
		for(unsigned int k=0;k<VolumeConvex::VERTEX_COUNT;++k)
			{t[k]=glm::dot(d,Vector( T_2.vertexGet(i_ref[k]) ) );}

		auto t_2=sort(Twins<float>
			{
			 (t[0]*pi[i_ref[1]] - t[1]*pi[i_ref[0]])/(pi[i_ref[1]] - pi[i_ref[0]])
			,(t[1]*pi[i_ref[2]] - t[2]*pi[i_ref[1]])/(pi[i_ref[2]] - pi[i_ref[1]])
			});

		auto ret=overlap(t_1,t_2);
#if 0
		if(ret)
			{
			fprintf(stderr,"T_1=[%.7g,%.7g,%.7g"
				,T_1.vertexGet(0).x,T_1.vertexGet(0).y,T_1.vertexGet(0).z);
			for(unsigned int k=1;k<VolumeConvex::VERTEX_COUNT;++k)
				{
				auto& v=T_1.vertexGet(k);
				fprintf(stderr,";\n%.7g,%.7g,%.7g",v.x,v.y,v.z);
				}
			fprintf(stderr,"]';\n");

			fprintf(stderr,"T_2=[%.7g,%.7g,%.7g"
				,T_2.vertexGet(0).x,T_2.vertexGet(0).y,T_2.vertexGet(0).z);
			for(unsigned int k=1;k<VolumeConvex::VERTEX_COUNT;++k)
				{
				auto& v=T_2.vertexGet(k);
				fprintf(stderr,";\n%.7g,%.7g,%.7g",v.x,v.y,v.z);
				}
			fprintf(stderr,"]';\ntriangle_intersect(T_1,T_2)\ntitle('%zu')\ndrawnow\n",g_event_count);
			}
#endif

		return ret;
		}
	}

const VolumeConvex::Face* VolumeConvex::cross(const Face& face) const
	{
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}

	auto face_current=facesBegin();
	while(face_current!=facesEnd())
		{
		if(triangle_intersect(face,*face_current))
			{return face_current;}
		++face_current;
		}
	return nullptr;
	}

void VolumeConvex::boundingBoxCompute() const
	{
	auto vertex_current=verticesBegin();
	if(vertex_current==verticesEnd())
		{
		m_bounding_box={{0,0,0},{0,0,0}};
		m_flags_dirty&=~BOUNDINGBOX_DIRTY;
		return;
		}
	m_bounding_box.m_min=Vector(*vertex_current);
	m_bounding_box.m_max=Vector(*vertex_current);

	++vertex_current;
	while(vertex_current!=verticesEnd())
		{
		m_bounding_box.m_min=glm::min(m_bounding_box.m_min,Vector(*vertex_current));
		m_bounding_box.m_max=glm::max(m_bounding_box.m_max,Vector(*vertex_current));

		++vertex_current;
		}

	m_flags_dirty&=~BOUNDINGBOX_DIRTY;
	}

void VolumeConvex::midpointCompute() const
	{
	m_mid={0,0,0,0};
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}
	auto face_current=facesBegin();
	while(face_current!=facesEnd())
		{
		auto a=Vector(face_current->vertexGet(0));
		auto b=Vector(face_current->vertexGet(1));
		auto c=Vector(face_current->vertexGet(2));
		auto n=face_current->m_normal_raw;

		auto s1=a+b;
		auto s2=b+c;
		auto s3=c+a;

		auto x=n*(s1*s1 + s2*s2 + s3*s3);
		m_mid+=Point(x,0);

		++face_current;
		}
	m_mid/=(48*volumeGet());
	m_mid.w=1;
	m_flags_dirty&=~MIDPOINT_DIRTY;
	}

void VolumeConvex::volumeCompute() const
	{
	m_volume=0;
	auto face_current=facesBegin();
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}

	while(face_current!=facesEnd())
		{
		m_volume+=glm::dot(Vector(face_current->vertexGet(0))
			,face_current->m_normal_raw);
		++face_current;
		}

	if(m_volume<0)
		{
		face_current=facesBegin();
		while(face_current!=facesEnd())
			{
			fprintf(stderr,"%.7g\n",glm::dot(Vector(face_current->vertexGet(0))
				,face_current->m_normal_raw));
			++face_current;
			}
		abort();
		}
	m_volume/=6;
	m_flags_dirty&=~VOLUME_DIRTY;
	}

void VolumeConvex::normalsFlip()
	{
	auto face_current=facesBegin();
	while(face_current!=facesEnd())
		{
		face_current->directionChange();
#ifndef NDEBUG
		auto& v_0=face_current->vertexGet(0);
		auto& v_1=face_current->vertexGet(1);
		auto& v_2=face_current->vertexGet(2);


		Vector u=Vector(v_1 - v_0);
		Vector v=Vector(v_2 - v_0);

		auto n=glm::cross(u,v);
		n=glm::normalize(n);
#endif
		++face_current;
		}
	m_flags_dirty|=FACES_NORMAL_DIRTY;
	}

void VolumeConvex::areaVisibleCompute() const
	{
	m_area_visible=0;
	if(m_flags_dirty&FACES_NORMAL_DIRTY)
		{facesNormalCompute();}

	auto i=m_faces_out.data();
	auto end=m_faces_out.data()+m_faces_out.size();
	while(i!=end)
		{
		m_area_visible+=glm::length(m_faces[*i].m_normal_raw);
		++i;
		}
	m_area_visible/=2;
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

	m_flags_dirty=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY
		|FACES_NORMAL_DIRTY|FACES_MIDPOINT_DIRTY|VOLUME_DIRTY
		|AREA_VISIBLE_DIRTY;

	auto face_current=facesBegin();
	while(face_current!=facesEnd())
		{
		face_current->parentSet(*this);
		++face_current;
		}

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
