//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"sphere_aggregate_loader.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "sphere_aggregate_loader.h"
#include "sphere_aggregate.h"
#include "config_command.h"

using namespace SnowflakeModel;

std::string SphereAggregateLoader::invoke(const ConfigCommand& cmd, const FileIn& source)
	{
	if(cmd.m_name=="sphere")
		{
		if(cmd.m_arguments.size()!=4)
			{throw "A sphere definition requires 4 arguments";}
		auto x=atof(cmd.m_arguments[0].c_str());
		auto y=atof(cmd.m_arguments[1].c_str());
		auto z=atof(cmd.m_arguments[2].c_str());
		auto r=atof(cmd.m_arguments[3].c_str());
		r_solid.subvolumeAdd(Sphere(Point(x,y,z,1.0f),r),0);
		}
	else
		{
		throw "VolumeLoader: Unknown command name";
		}
	return std::string("");
	}
