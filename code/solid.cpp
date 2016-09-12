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

using namespace SnowflakeModel;

void Solid::merge(const Matrix& T,const Solid& volume,bool mirrored)
	{
	auto n=m_subvolumes.size();
		{
		auto subvolume=volume.subvolumesBegin();
		while(subvolume!=volume.subvolumesEnd())
			{
			m_subvolumes.push_back(*subvolume);
			++subvolume;
			}
		}

		{
		auto subvolume=subvolumesBegin()+n;
		while(subvolume!=subvolumesEnd())
			{
			subvolume->transform(T);
			if(mirrored)
				{subvolume->normalsFlip();}
			++subvolume;
			}
		}
	m_n_faces_tot+=volume.facesCount();
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY|VOLUME_DIRTY|DMAX_DIRTY;
	}

void Solid::merge(const Solid& volume)
	{
	auto subvolume=volume.subvolumesBegin();
	while(subvolume!=volume.subvolumesEnd())
		{
		m_subvolumes.push_back(*subvolume);
		++subvolume;
		}
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY|VOLUME_DIRTY|DMAX_DIRTY;

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
	m_mid={0,0,0,0};
	while(subvolume!=subvolumesEnd())
		{
		m_mid+=subvolume->midpointGet();
		++subvolume;
		}
	m_mid/=subvolumesCount();
	m_mid=Point{Vector(m_mid),1};
	m_flags_dirty&=~MIDPOINT_DIRTY;
	}

void Solid::geometrySample(VoxelBuilder& builder) const
	{
	auto subvolume=subvolumesBegin();
	while(subvolume!=subvolumesEnd())
		{
		subvolume->geometrySample(builder);
		++subvolume;
		}
	}

const VolumeConvex* Solid::inside(const Point& v) const
	{
	auto subvolume=subvolumesBegin();
	while(subvolume!=subvolumesEnd())
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
	while(subvolume!=subvolumesEnd())
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
	while(subvolume!=subvolumesEnd())
		{
		subvolume->transform(T);
		++subvolume;
		}
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY;
	}

void Solid::centerBoundingBoxAt(const Point& pos_new)
	{
	auto bb_mid=Point(0.5f*(boundingBoxGet().m_max+boundingBoxGet().m_min),1);
	Matrix T;
	T=glm::translate(T,Vector(pos_new-bb_mid));
	auto subvolume=subvolumesBegin();
	while(subvolume!=subvolumesEnd())
		{
		subvolume->transform(T);
		++subvolume;
		}
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY;
	}

void Solid::rMaxCompute() const
	{
	m_r_max=0;
	auto mid=midpointGet();
	auto subvolume=subvolumesBegin();
	while(subvolume!=subvolumesEnd())
		{
		auto v_current=subvolume->verticesBegin();
		while(v_current!=subvolume->verticesEnd())
			{
			auto r_temp=glm::distance(*v_current,mid);
			if(r_temp > m_r_max)
				{m_r_max=r_temp;}
			++v_current;
			}
		++subvolume;
		}
	m_flags_dirty&=~RMAX_DIRTY;
	}

void Solid::volumeCompute() const
	{
	m_volume=0;
	auto subvolume=subvolumesBegin();
	while(subvolume!=subvolumesEnd())
		{
		m_volume+=subvolume->volumeGet();
		++subvolume;
		}
	m_flags_dirty&=~VOLUME_DIRTY;
	}

bool SnowflakeModel::overlap(const Solid& v_a,const Solid& v_b)
	{
	//	Vertices from v_a inside v_b?
		{
		auto subvolume=v_a.subvolumesBegin();
		while(subvolume!=v_a.subvolumesEnd())
			{
			auto vertex=subvolume->verticesBegin();
			while(vertex!=subvolume->verticesEnd())
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
		while(subvolume!=v_b.subvolumesEnd())
			{
			auto vertex=subvolume->verticesBegin();
			while(vertex!=subvolume->verticesEnd())
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
		while(subvolume!=v_a.subvolumesEnd())
			{
			auto face=subvolume->facesBegin();
			while(face!=subvolume->facesEnd())
				{
				if(v_b.cross(*face))
					{return 1;}
				++face;
				}
			++subvolume;
			}
		}

/*	if(v_b.inside(v_a.midpointGet()) || v_a.inside(v_b.midpointGet()))
		{return 1;}*/
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

void Solid::dMaxCompute() const noexcept
	{
	}
