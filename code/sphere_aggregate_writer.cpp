//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"sphere_aggregate_writer.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "sphere_aggregate_writer.h"
#include "sphere_aggregate.h"
#include "file_out.h"

using namespace SnowflakeModel;

SphereAggregateWriter::SphereAggregateWriter(FileOut& dest):r_dest(dest)
	{}

void SphereAggregateWriter::write(const SphereAggregate& sphere_aggregate)
	{
	auto subvol_current=sphere_aggregate.subvolumesBegin();
	while(subvol_current!=sphere_aggregate.subvolumesEnd())
		{
		auto pos=subvol_current->midpointGet();
		auto r=subvol_current->radiusGet();
		r_dest.printf("sphere(%.7g,%.7g,%.7g,%.7g)\n",pos.x,pos.y,pos.z,r);
		++subvol_current;
		}
	}
