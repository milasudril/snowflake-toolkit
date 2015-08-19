#ifdef __WAND__
target[name[snowflake_test] type[application] platform[;GNU/Linux]]
#endif

#include "aggregate.h"
#include "solid_builder.h"
#include "solid_writer.h"
#include "solid_loader.h"
#include "solid.h"
#include "voxelbuilder_adda.h"
#include "config_parser.h"
#include "file_out.h"

#include <cmath>

int main()
	{
	SnowflakeModel::Solid crystal_base;
		{
		SnowflakeModel::FileIn input(stdin);
		SnowflakeModel::ConfigParser parser(input);
		SnowflakeModel::SolidLoader loader(crystal_base);
		parser.commandsRead(loader);
		}

	SnowflakeModel::Aggregate bullet;
	
	SnowflakeModel::AggregateNode branches[5];
	
	const float length_cc=4;

	for(size_t k=0;k<5;++k)
		{
		branches[k].grainGet().solidSet(crystal_base);
		}
	bullet.rootGet().grainGet().solidSet(crystal_base);
	const float angle=acos(0);
	float pi=acos(-1);
	
	bullet.childAppend(0.5f*SnowflakeModel::Vector{length_cc,0,0}
		,branches[0],0.5f*SnowflakeModel::Vector{length_cc,0,0},0,pi,angle);
	bullet.childAppend(0.5f*SnowflakeModel::Vector{length_cc,0,0}
		,branches[1],0.5f*SnowflakeModel::Vector{length_cc,0,0},0,pi,0);
	bullet.childAppend(0.5f*SnowflakeModel::Vector{length_cc,0,0}
		,branches[2],0.5f*SnowflakeModel::Vector{length_cc,0,0},0,pi,-angle);
	bullet.childAppend(0.5f*SnowflakeModel::Vector{length_cc,0,0}
		,branches[3],0.5f*SnowflakeModel::Vector{length_cc,0,0},0,angle,0);
	bullet.childAppend(0.5f*SnowflakeModel::Vector{length_cc,0,0}
		,branches[4],0.5f*SnowflakeModel::Vector{length_cc,0,0},0,-angle,0);
		
	SnowflakeModel::Solid m;
	bullet.grainsVisit(SnowflakeModel::SolidBuilder(m));
	m.centerBoundingBoxAt(SnowflakeModel::Point(0,0,0,1));
	

	SnowflakeModel::FileOut output(stdout);
	SnowflakeModel::VoxelbuilderAdda builder(output,64,64,64,m.boundingBoxGet());
	m.geometrySample(builder);
	
	return 0;
	}
