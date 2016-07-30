//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"config_command.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_CONFIGCOMMAND_H
#define SNOWFLAKEMODEL_CONFIGCOMMAND_H

#include <string>
#include <vector>

namespace SnowflakeModel
	{
	struct ConfigCommand
		{
		std::string m_name;
		std::vector<std::string> m_arguments;
		};
	}

#endif
