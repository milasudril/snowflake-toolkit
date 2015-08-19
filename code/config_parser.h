#ifdef __WAND__
target[name[config_parser.h] type[include]]
dependency[config_parser.o]
#endif

#ifndef SNOWFLAKEMODEL_CONFIGPARSER_H
#define SNOWFLAKEMODEL_CONFIGPARSER_H

#include "file_in.h"

namespace SnowflakeModel
	{
	class ConfigCommandHandler;
	
	class ConfigParser
		{
		public:
			ConfigParser(FileIn& source);
			void commandsRead(ConfigCommandHandler& handler);
			
		private:
			FileIn& r_source;
		};
	};

#endif
