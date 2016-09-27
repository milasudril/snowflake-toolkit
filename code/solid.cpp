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
//	Copy input volume.
	auto v_temp=volume;
//	Transform it
	v_temp.transform(T,mirrored);

//	Accumulate quantities
	m_n_faces_tot+=v_temp.facesCount();
	m_volume+=v_temp.volumeGet();

//	Update extreme vertices
	extremaUpdate(v_temp);

	boundingBoxUpdate(v_temp);

//	Merge
	auto v_temp_begin=v_temp.subvolumesBegin();
	auto v_temp_end=v_temp.subvolumesEnd();
	while(v_temp_begin!=v_temp_end)
		{
	//	subvolumeAdd does too mutch in this case
		m_subvolumes.push_back(std::move(*v_temp_begin));
		++v_temp_begin;
		}

//	Mark things as dirty
	m_flags_dirty|=MIDPOINT_DIRTY|RMAX_DIRTY;
	}

void Solid::merge(const Solid& volume,double overlap_est,size_t overlap_count)
	{
	extremaUpdate(volume);
	boundingBoxUpdate(volume);
	auto subvolume=volume.subvolumesBegin();
	auto vols_end=volume.subvolumesEnd();
	while(subvolume!=vols_end)
		{
	//	subvolumeAdd does too mutch in this case
		m_subvolumes.push_back(*subvolume);
		++subvolume;
		}
	m_flags_dirty|=MIDPOINT_DIRTY|RMAX_DIRTY;
	m_volume+=volume.volumeGet() - overlap_est;
	m_overlap_count+=overlap_count;
	m_n_faces_tot+=volume.facesCount();
	}

Vector SnowflakeModel::strechToBoundingBox(const Vector& v,const Solid& V) noexcept
	{
	const BoundingBox& bb=V.boundingBoxGet();
	auto v_emax=extentMax(glm::abs(v));
	auto s=glm::max(glm::abs(bb.m_min[v_emax.second])
		,glm::abs(bb.m_max[v_emax.second]));
	return s*v/v_emax.first;
	}

Vector SnowflakeModel::strechToSurface(const Vector& v,const Solid& V,float tolerance) noexcept
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

void Solid::transform(const Matrix& T,bool mirrored) noexcept
	{
	auto subvolume=subvolumesBegin();
	auto subvols_end=subvolumesEnd();
	while(subvolume!=subvols_end)
		{
		subvolume->transform(T);
		if(mirrored)
			{subvolume->normalsFlip();}
		++subvolume;
		}
	m_extrema.first=T*m_extrema.first;
	m_extrema.second=T*m_extrema.second;
//TODO (perf) Use determinant of T to compute the new volume
	m_flags_dirty|=BOUNDINGBOX_DIRTY|MIDPOINT_DIRTY|RMAX_DIRTY|VOLUME_DIRTY|DMAX_DIRTY;
	}

void Solid::normalsFlip() noexcept
	{
	auto subvolume=subvolumesBegin();
	auto subvols_end=subvolumesEnd();
	while(subvolume!=subvols_end)
		{
		subvolume->normalsFlip();
		++subvolume;
		}
	}

void Solid::boundingBoxCompute() const noexcept
	{
	m_bounding_box={{INFINITY,INFINITY,INFINITY,1.0f},{-INFINITY,-INFINITY,-INFINITY,1.0f}};
	boundingBoxUpdate(*this);
	m_flags_dirty&=~BOUNDINGBOX_DIRTY;
	}

void Solid::midpointCompute() const noexcept
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

const VolumeConvex* Solid::inside(const Point& v) const noexcept
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


void Solid::centerCentroidAt(const Point& pos_new) noexcept
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
	m_extrema.first=T*m_extrema.first;
	m_extrema.second=T*m_extrema.second;
	m_mid=pos_new; //By definition
	m_bounding_box.m_min=T*m_bounding_box.m_min;
	m_bounding_box.m_max=T*m_bounding_box.m_max;
	}

void Solid::centerBoundingBoxAt(const Point& pos_new) noexcept
	{
	auto bb_mid=boundingBoxGet().centerGet();
	Matrix T;
	T=glm::translate(T,Vector(pos_new-bb_mid));
	auto subvolume=subvolumesBegin();
	auto vol_end=subvolumesEnd();
	while(subvolume!=vol_end)
		{
		subvolume->transform(T);
		++subvolume;
		}
	m_extrema.first=T*m_extrema.first;
	m_extrema.second=T*m_extrema.second;
	m_mid=T*m_mid;
	m_bounding_box.m_min=T*m_bounding_box.m_min;
	m_bounding_box.m_max=T*m_bounding_box.m_max;
	}

void Solid::rMaxCompute() const noexcept
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

void Solid::volumeCompute() const noexcept
	{
	double volume=0;
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

size_t SnowflakeModel::overlap(const Solid& v_a,const Solid& v_b
	,size_t subvols,double& vol_overlap) noexcept
	{
	if(!overlap(v_a.boundingBoxGet(),v_b.boundingBoxGet()))
		{
		vol_overlap=0;
		return 0;
		}

	double vol_overlap_temp=0;
	size_t cross_count=0;

	auto subvolume=v_a.subvolumesBegin();
	auto vol_end=v_a.subvolumesEnd();
	auto subvols_b_end=v_b.subvolumesEnd();
	auto subvol_b_0=v_b.subvolumesBegin();
	while(subvolume!=vol_end)
		{
		auto V=subvolume->volumeGet();
		auto subvol_b=subvol_b_0;
		while(subvol_b!=subvols_b_end)
			{
			if( overlap(*subvol_b,*subvolume) )
				{
				++cross_count;
				if(cross_count > subvols)
					{return cross_count;}
			//	This is a guesstimate of the actual overlap. It is possible
			//	to find the true value, but that may require a remeshing
			//	step.
				vol_overlap_temp+=0.1*std::min(V,subvol_b->volumeGet());
				}
			++subvol_b;
			}
		++subvolume;
		}
	vol_overlap=vol_overlap_temp;
	return cross_count;
	}

bool SnowflakeModel::overlap(const Solid& v_a,const Solid& v_b) noexcept
	{
	if(!overlap(v_a.boundingBoxGet(),v_b.boundingBoxGet()))
		{return 0;}

	auto subvolume=v_a.subvolumesBegin();
	auto vol_end=v_a.subvolumesEnd();
	auto subvols_b_end=v_b.subvolumesEnd();
	auto subvol_b_0=v_b.subvolumesBegin();
	while(subvolume!=vol_end)
		{
		auto subvol_b=subvol_b_0;
		while(subvol_b!=subvols_b_end)
			{
			if(overlap(*subvol_b,*subvolume))
				{return 1;}
			++subvol_b;
			}
		++subvolume;
		}
	return 0;
	}

void Solid::write(const char* id,DataDump& dump) const
	{
	auto group=dump.groupCreate(id);
	std::string group_name(id);
	dump.write((group_name + "/mirror_flags").c_str(),&m_mirror_flags,1);
	dump.write((group_name + "/overlap_count").c_str(),&m_overlap_count,1);

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
		dump.write((group_name+"/volume").c_str(),&m_volume,1);
	}

Solid::Solid(const DataDump& dump,const char* name):
	m_extrema{{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f,1.0f}}
	{
	clear();
	std::string group_name(name);
	m_mirror_flags=dump.arrayGet<decltype(m_mirror_flags)>
		((group_name+"/mirror_flags").c_str()).at(0);

	m_overlap_count=dump.arrayGet<decltype(m_overlap_count)>
		
		((group_name+"/overlap_count").c_str()).at(0);

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
			subvolumeAdd(VolumeConvex(dump,group_name_current.c_str()),0,0);
			});
		}
	m_volume=dump.arrayGet<decltype(m_volume)>
		((group_name+"/volume").c_str()).at(0);
	}

void Solid::extremaUpdate(const VolumeConvex& vol) const noexcept
	{
//	It is sufficent to check all vertices in *this against
//	all vertices in v. This is because we are adding v to *this,
//	which is the reason for the change in extrema.

	auto extrema_in=extremaGet(); //retrieve current values.
	auto d_max=glm::distance(extrema_in.first,extrema_in.second);

	auto subvol_this=subvolumesBegin();
	auto subvol_this_end=subvolumesEnd();
	auto v_begin_0=vol.verticesBegin();
	auto v_end=vol.verticesEnd();

	if(subvolumesCount()==0)
		{
	//	There are no subvolumes yet. Only consider vertices in vol.
		auto v_begin=v_begin_0;
		while(v_begin!=v_end)
			{
			auto v=*v_begin;
			auto w_ptr=v_begin + 1;
			while(w_ptr!=v_end)
				{
				auto w=*w_ptr;
				auto d=glm::distance(v,w);
				if(d>d_max)
					{
					d_max=d;
					extrema_in={v,w};
					}
				++w_ptr;
				}
			++v_begin;
			}
		m_extrema=extrema_in;
		return;
		}


//	TODO (perf) is this the most optimal loop order?
	while(subvol_this!=subvol_this_end)
		{
		auto v_this=subvol_this->verticesBegin();
		auto v_this_end=subvol_this->verticesEnd();
		while(v_this!=v_this_end)
			{
			auto v=*v_this;
			auto v_begin=v_begin_0;
			while(v_begin!=v_end)
				{
				auto w=*v_begin;
				auto d=glm::distance(v,w);
				if(d>d_max)
					{
					d_max=d;
					extrema_in={v,w};
					}
				++v_begin;
				}
			++v_this;
			}
		++subvol_this;
		}
	m_extrema=extrema_in;
	}

void Solid::extremaUpdate(const Solid& solid) const noexcept
	{
	auto s_begin=solid.subvolumesBegin();
	auto s_end=solid.subvolumesEnd();
	while(s_begin!=s_end)
		{
		extremaUpdate(*s_begin);
		++s_begin;
		}
	}

void Solid::boundingBoxUpdate(const VolumeConvex& v) const noexcept
	{
//	Compute the new bounding box as if v were added to this solid
	auto& bb=v.boundingBoxGet();
	m_bounding_box.m_min=glm::min(bb.m_min,m_bounding_box.m_min);
	m_bounding_box.m_max=glm::max(bb.m_max,m_bounding_box.m_max);
	}


void Solid::boundingBoxUpdate(const Solid& solid) const noexcept
	{
//	Compute the new bounding box as if solid were added to this solid
	auto s_begin=solid.subvolumesBegin();
	auto s_end=solid.subvolumesEnd();
	while(s_begin!=s_end)
		{
		boundingBoxUpdate(*s_begin);
		++s_begin;
		}
	}

void Solid::dMaxCompute() const noexcept
	{
	m_extrema={{0.0f,0.0f,0.0f,1.0f},{0.0f,0.0f,0.0f,1.0f}};
	extremaUpdate(*this);
	m_flags_dirty&=~DMAX_DIRTY;
	}

std::pair<Triangle,float> Solid::shoot(const Point& source,const Vector& direction
	,float cos_pass_angle) const noexcept
	{
	auto subvols_begin=subvolumesBegin();
	auto subvols_end=subvolumesEnd();

	assert(subvols_begin!=subvols_end);
	--subvols_end;
	auto ret=subvols_end->shoot(source,direction,cos_pass_angle);
	

	while(subvols_begin!=subvols_end)
		{
		--subvols_end;
		auto d_box=distance(source,subvols_end->boundingBoxGet());
		if(d_box < ret.second)
			{
			auto res=subvols_end->shoot(source,direction,cos_pass_angle);
			if(res.second < ret.second)
				{ret=res;}
			}
		}
	return ret;
	}
