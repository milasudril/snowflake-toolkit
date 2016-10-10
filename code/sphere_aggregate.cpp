//@	{"targets":[{"name":"sphere_aggregate.o","type":"object"}]}

#include "sphere_aggregate.h"

using namespace SnowflakeModel;

//TODO: This should be a template

bool SnowflakeModel::overlap(const SphereAggregate& v_a, const SphereAggregate& v_b)
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

std::pair<Point,Vector> SphereAggregate::shoot(const Point& source
	,const Vector& direction
	,float E_0,float decay_distance) const noexcept
	{
	auto subvols_begin=subvolumesBegin();
	auto subvols_end=subvolumesEnd();

	if(subvols_begin==subvols_end)
		{return std::make_pair(Point(0.0f,0.0f,0.0f,1.0f),Vector(1.0f,0.0f,0.0f));}

	--subvols_end;

	auto d=subvols_end->shoot(source,direction,E_0,decay_distance);
	auto obj_min=subvols_end;
	
	while(subvols_begin!=subvols_end)
		{
		--subvols_end;
		auto d_box=distance(source,subvols_end->boundingBoxGet());
		if(d_box < d)
			{
			auto d_new=subvols_end->shoot(source,direction,E_0,decay_distance);
			if(d_new < d)
				{
				d=d_new;
				obj_min=subvols_end;
				}
			}
		}
	auto point=source + Point(d*direction,1.0f);
	auto normal=glm::normalize(Vector(point - obj_min->midpointGet()));
	return std::make_pair(point,normal);
	}

void SphereAggregate::geometrySample(VoxelBuilder& builder) const
	{
	auto subvols_begin=subvolumesBegin();
	auto subvols_end=subvolumesEnd();

	while(subvols_begin!=subvols_end)
		{
		auto seed=subvols_begin->floodfillSeedGet(builder);
	//	subvols_begin->geometrySample(builder);
		++subvols_begin;
		}
	}
