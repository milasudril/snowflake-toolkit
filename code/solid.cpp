//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "solid.h"
#include <algorithm>

using namespace SnowflakeModel;

void Solid::merge(const Matrix& T,const Solid& volume,bool mirrored)
	{
	auto n=m_subvolumes.size();
		{
	//	Copy all volumes
		auto subvolume=volume.subvolumesBegin();
		auto vols_end=volume.subvolumesEnd();
		while(subvolume!=vols_end)
			{
		//	SubvolumeAdd not needed. We must do these things later due to the 
		//	transformation.
			m_subvolumes.push_back(*subvolume);
			++subvolume;
			}
		}

		{
	//	Transform them
		auto subvolume=subvolumesBegin()+n;
		auto vols_end=subvolumesEnd();
		while(subvolume!=vols_end)
			{
			subvolume->transform(T);
			if(mirrored)
				{subvolume->normalsFlip();}
			dMaxCompute(*subvolume);
			m_volume+=subvolume->volumeGet();
			++subvolume;
			}
		}
	m_n_faces_tot+=volume.facesCount();
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY|DMAX_DIRTY;
	}

void Solid::merge(const Solid& volume)
	{
	auto subvolume=volume.subvolumesBegin();
	auto vols_end=volume.subvolumesEnd();
	while(subvolume!=vols_end)
		{
	//	subvolumeAdd not needed here (We correct values after the loop)
		m_subvolumes.push_back(*subvolume);
	//	This must be done here [Incremental computation based on subvolumes]
		dMaxCompute(*subvolume);
		++subvolume;
		}
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY;
	m_volume+=volume.volumeGet();
	m_n_faces_tot+=volume.facesCount();
	}

Vector SnowflakeModel::strechToBoundingBox(const Vector& v,const Solid& V)
	{
	const BoundingBox& bb=V.boundingBoxGet();
	auto v_emax=extentMax(glm::abs(v));
	auto s=glm::max(glm::abs(bb.m_min[v_emax.second])
		,glm::abs(bb.m_max[v_emax.second]));
	return s*v/v_emax.first;
	}

Vector SnowflakeModel::strechToSurface(const Vector& v,const Solid& V,float tolerance)
	{
	auto ret=strechToBoundingBox(v,V);
	float lower=0;
	float upper=2;

	while(upper-lower>tolerance && upper > lower)
		{
		auto t=0.5f*(upper + lower);
 		if(V.inside(Point(t*ret,1)))
			{lower=t;}
		else
			{upper=t;}
		}
	return 0.5f*(upper+lower)*ret;
	}

void Solid::transform(const Matrix& T,bool mirrored)
	{
	auto subvolume=subvolumesBegin();
	while(subvolume!=subvolumesEnd())
		{
		subvolume->transform(T);
		if(mirrored)
			{subvolume->normalsFlip();}
		++subvolume;
		}
//TODO (perf) Use determinant of T to compute the new volume
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY|VOLUME_DIRTY|DMAX_DIRTY;
	}

void Solid::normalsFlip()
	{
	auto subvolume=subvolumesBegin();
	while(subvolume!=subvolumesEnd())
		{
		subvolume->normalsFlip();
		++subvolume;
		}
	}

void Solid::boundingBoxCompute() const
	{
	auto subvolume=subvolumesBegin();
	if(subvolume==subvolumesEnd())
		{
		m_bounding_box={{0,0,0},{0,0,0}};
		m_flags_dirty&=~BOUNDINGBOX_DIRTY;
		return;
		}

	m_bounding_box=subvolume->boundingBoxGet();
	++subvolume;

	while(subvolume!=subvolumesEnd())
		{
		auto& bb_temp=subvolume->boundingBoxGet();
		m_bounding_box.m_min=glm::min(m_bounding_box.m_min,bb_temp.m_min);
		m_bounding_box.m_max=glm::max(m_bounding_box.m_max,bb_temp.m_max);
		++subvolume;
		}

	m_flags_dirty&=~BOUNDINGBOX_DIRTY;
	}

void Solid::midpointCompute() const
	{
	auto subvolume=subvolumesBegin();
	glm::vec4 mid{0.0f,0.0f,0.0f,0.0f};
	auto vol_end=subvolumesEnd();
	while(subvolume!=vol_end)
		{
		mid+=subvolume->midpointGet();
		++subvolume;
		}
	mid/=subvolumesCount();
	m_mid=Point{Vector(mid),1};
	m_flags_dirty&=~MIDPOINT_DIRTY;
	}

void Solid::geometrySample(VoxelBuilder& builder) const
	{
	auto subvolume=subvolumesBegin();
	auto vol_end=subvolumesEnd();
	while(subvolume!=vol_end)
		{
		subvolume->geometrySample(builder);
		++subvolume;
		}
	}

const VolumeConvex* Solid::inside(const Point& v) const
	{
	auto subvolume=subvolumesBegin();
	auto vol_end=subvolumesEnd();
	while(subvolume!=vol_end)
		{
		if(subvolume->inside(v))
			{return subvolume;}
		++subvolume;
		}
	return nullptr;
	}


const VolumeConvex* Solid::cross(const VolumeConvex::Face& f) const
	{
	auto subvolume=subvolumesBegin();
	auto vol_end=subvolumesEnd();
	while(subvolume!=vol_end)
		{
		if(subvolume->cross(f))
			{return subvolume;}
		++subvolume;
		}
	return nullptr;
	}


void Solid::centerCentroidAt(const Point& pos_new)
	{
	auto subvolume=subvolumesBegin();
	Matrix T;
	T=glm::translate(T,Vector(pos_new-midpointGet()));
	auto vol_end=subvolumesEnd();
	while(subvolume!=vol_end)
		{
		subvolume->transform(T);
		++subvolume;
		}
//TODO (perf) We can deduce a new midpoint [but floating point arithmetic...]
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY;
	}

void Solid::centerBoundingBoxAt(const Point& pos_new)
	{
	auto bb_mid=Point(0.5f*(boundingBoxGet().m_max+boundingBoxGet().m_min),1);
	Matrix T;
	T=glm::translate(T,Vector(pos_new-bb_mid));
	auto subvolume=subvolumesBegin();
	auto vol_end=subvolumesEnd();
	while(subvolume!=vol_end)
		{
		subvolume->transform(T);
		++subvolume;
		}
//TODO (perf) We can deduce a new midpoint [but floating point arithmetic...]
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY;
	}

void Solid::rMaxCompute() const
	{
	float r_max=0;
	auto mid=midpointGet();
	auto subvolume=subvolumesBegin();
	auto vol_end=subvolumesEnd();
	while(subvolume!=vol_end)
		{
		auto v_current=subvolume->verticesBegin();
		auto v_end=subvolume->verticesEnd();
		while(v_current!=v_end)
			{
			r_max=std::max(glm::distance(*v_current,mid), r_max);
			++v_current;
			}
		++subvolume;
		}
	m_r_max=r_max;
	m_flags_dirty&=~RMAX_DIRTY;
	}

void Solid::volumeCompute() const
	{
	float volume=0;
	auto subvolume=subvolumesBegin();
	auto subvols_end=subvolumesEnd();
	while(subvolume!=subvols_end)
		{
		volume+=subvolume->volumeGet();
		++subvolume;
		}
	m_volume=volume;
	m_flags_dirty&=~VOLUME_DIRTY;
	}

bool SnowflakeModel::overlap(const Solid& v_a,const Solid& v_b)
	{
	//	Vertices from v_a inside v_b?
		{
		auto subvolume=v_a.subvolumesBegin();
		auto vol_end=v_a.subvolumesEnd();
		while(subvolume!=vol_end)
			{
			auto vertex=subvolume->verticesBegin();
			auto v_end=subvolume->verticesEnd();
			while(vertex!=v_end)
				{
				if(v_b.inside(*vertex)!=nullptr)
					{return 1;}
				++vertex;
				}
			++subvolume;
			}
		}

	//	Vertices from v_b inside v_a?
		{
		auto subvolume=v_b.subvolumesBegin();
		auto vol_end=v_b.subvolumesEnd();
		while(subvolume!=vol_end)
			{
			auto vertex=subvolume->verticesBegin();
			auto v_end=subvolume->verticesEnd();
			while(vertex!=v_end)
				{
				if(v_a.inside(*vertex)!=nullptr)
					{return 1;}
				++vertex;
				}
			++subvolume;
			}
		}

	//	Triangle overlap
		{
		auto subvolume=v_a.subvolumesBegin();
		auto vol_end=v_a.subvolumesEnd();
		while(subvolume!=vol_end)
			{
			auto face=subvolume->facesBegin();
			auto face_end=subvolume->facesEnd();
			while(face!=face_end)
				{
				if(v_b.cross(*face))
					{return 1;}
				++face;
				}
			++subvolume;
			}
		}
	return 0;
	}

void Solid::write(const char* id,DataDump& dump) const
	{
	auto group=dump.groupCreate(id);
	std::string group_name(id);
	dump.write((group_name + "/mirror_flags").c_str(),&m_mirror_flags,1);
	
		{
		size_t k=0;
		auto deformations_begin=m_deformation_templates.data();
		auto deformations_end=deformations_begin + m_deformation_templates.size();
		auto defgroup=dump.groupCreate((group_name + "/deformation_templates").c_str());
		auto defgroup_name=group_name + "/deformation_templates/"; 
		while(deformations_begin!=deformations_end)
			{
			char id[32];
			sprintf(id,"%016zx",k);
			auto group_name_current=defgroup_name + id;
			deformations_begin->write(group_name_current.c_str(),dump);
			++deformations_begin;
			++k;
			}
		}

		{
		size_t k=0;
		auto subvols_begin=subvolumesBegin();
		auto subvols_end=subvolumesEnd();
		auto defgroup=dump.groupCreate((group_name + "/subvolumes").c_str());
		auto defgroup_name=group_name + "/subvolumes/"; 
		while(subvols_begin!=subvols_end)
			{
			char id[32];
			sprintf(id,"%016zx",k);
			auto group_name_current=defgroup_name + id;
			subvols_begin->write(group_name_current.c_str(),dump);
			++subvols_begin;
			++k;
			}
		}
	}

Solid::Solid(const DataDump& dump,const char* name)
	{
	clear();
	std::string group_name(name);
	m_mirror_flags=dump.arrayGet<decltype(m_mirror_flags)>
		((group_name+"/mirror_flags").c_str()).at(0);

		{
		auto defgroup_name=group_name + "/deformation_templates";
		auto defgroup=dump.groupOpen(defgroup_name.c_str());
		defgroup_name+='/';
		
		dump.iterate(*defgroup,[&dump,&defgroup_name,this]
			(const char* group_name)
			{
			auto group_name_current=defgroup_name + group_name;
			m_deformation_templates.push_back(SolidDeformation(dump,group_name_current.c_str()));
			});
		}

		{
		auto defgroup_name=group_name + "/subvolumes";
		auto defgroup=dump.groupOpen(defgroup_name.c_str());
		defgroup_name+='/';		
		dump.iterate(*defgroup,[&dump,&defgroup_name,this]
			(const char* group_name)
			{
			auto group_name_current=defgroup_name + group_name;
			subvolumeAdd(VolumeConvex(dump,group_name_current.c_str()));
			});
		}
	}

void Solid::dMaxCompute(const VolumeConvex& vol) noexcept
	{
	auto dmax_a=m_dmax_a; //Initial boundary points
	auto dmax_b=m_dmax_b;

	auto d_max=glm::distance(dmax_a,dmax_b);
//	Init case
	if(d_max < 1e-7f)
		{
		auto v_current=vol.verticesBegin();
		auto v_end=vol.verticesEnd();
		while(v_current!=v_end)
			{
			auto v=*v_current;
			auto w=v_current + 1;
			while(w!=v_end)
				{
				auto b=*w;
				auto d=glm::distance(v,b);
				if(d>d_max)
					{
					dmax_a=v;
					dmax_b=b;
					d_max=d;
					}
				++w;
				}
			++v_current;
			}
		m_dmax_a=dmax_a;
		m_dmax_b=dmax_b;
		return;
		}

//	The new value of D_max has to be the distance between a point
//	in this object, and a point in the other object, if the addition
//	of a subvolume change D_max at all. There are two points we can move:
//	A and B, but not both.

	auto b_new=dmax_b;
	auto d_b=d_max;
//	Can we find a point in v, that is longer from a than b?
		{
		auto v_current=vol.verticesBegin();
		auto v_end=vol.verticesEnd();
		while(v_current!=v_end)
			{
			auto v=*v_current;
			auto d=glm::distance(v,dmax_a);
			if(d>d_b)
				{
				d_b=d;
				b_new=v;
				}
			++v_current;
			}
		}

//	Can we find a point in v, that is longer from b than a?
	auto a_new=dmax_a;
	auto d_a=d_max;
		{
		auto v_current=vol.verticesBegin();
		auto v_end=vol.verticesEnd();
		while(v_current!=v_end)
			{
			auto v=*v_current;
			auto d=glm::distance(v,dmax_b);
			if(d>d_a)
				{
				d_a=d;
				a_new=v;
				}
			++v_current;
			}
		}

//	Now choose the largest of d_max, d_a, and d_b
	float d[3]={d_max,d_a,d_b};
	auto p_max=std::max_element(d,d+3) - d;
	if(p_max==0)
		{return;}
	if(p_max==1)
		{
		d_max=d_a;
		m_dmax_a=a_new;
		return;
		}
	d_max=d_b;
	m_dmax_a=b_new;
	}
