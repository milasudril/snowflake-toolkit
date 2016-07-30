//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"config_commandhandler.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_CONFIGCOMMANDHANDLER_H
#define SNOWFLAKEMODEL_CONFIGCOMMANDHANDLER_H

#include <string>

namespace SnowflakeModel
	{
	class ConfigCommand;
	class FileIn;
	class ConfigCommandHandler
		{
		public:
			virtual std::string invoke(const ConfigCommand& command
				,const FileIn& source)=0;
		};
	}

#endif

