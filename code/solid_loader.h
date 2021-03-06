//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"solid_loader.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid_loader.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_SOLIDLOADER_H
#define SNOWFLAKEMODEL_SOLIDLOADER_H

#include "file_in.h"
#include "config_commandhandler.h"

namespace SnowflakeModel
	{
	class Solid;
	class VolumeConvex;

	class SolidLoader:public ConfigCommandHandler
		{
		public:
			SolidLoader(Solid& solid):r_solid(solid){}
			std::string invoke(const ConfigCommand& cmd,const FileIn& source);

		private:
			Solid& r_solid;
			VolumeConvex* r_vc_current;
		};
	}

#endif
