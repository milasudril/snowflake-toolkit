#ifdef __WAND__
target[name[config_command.h] type[include]]
#endif

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
