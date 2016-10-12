//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"sphere_aggregate_loader.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"sphere_aggregate_loader.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_SPHEREAGGREGATELOADER_H
#define SNOWFLAKEMODEL_SPHEREAGGREGATELOADER_H

#include "file_in.h"
#include "config_commandhandler.h"

namespace SnowflakeModel
	{
	class SphereAggregate;
	class VolumeConvex;

	class SphereAggregateLoader:public ConfigCommandHandler
		{
		public:
			SphereAggregateLoader(SphereAggregate& solid):r_solid(solid){}
			std::string invoke(const ConfigCommand& cmd,const FileIn& source);

		private:
			SphereAggregate& r_solid;
		};
	}

#endif
