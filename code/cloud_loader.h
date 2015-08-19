#ifdef __WAND__
target[name[cloud_loader.h] type[include]]
dependency[cloud_loader.o]
#endif

#ifndef SNOWFLAKEMODEL_CLOUDLOADER_H
#define SNOWFLAKEMODEL_CLOUDLOADER_H

#include "config_commandhandler.h"
#include "vector.h"

#include <map>

namespace SnowflakeModel
	{
	class Cloud;
	class Solid;
	class Aggregate;
	class AggregateNode;
	
	/**\brief A CloudLoader loads Cloud data from a cloud file.
	*/
	class CloudLoader:public ConfigCommandHandler
		{
		public:
			CloudLoader(Cloud& cloud
				,const std::map<std::string,std::string>& varlist)
				:r_cloud(cloud),r_varlist(varlist)
				{}

			std::string invoke(const ConfigCommand& invoke,const FileIn& source);
			
			std::map<std::string,std::string>::const_iterator varsBegin() const
				{return m_varlist.begin();}
			
			std::map<std::string,std::string>::const_iterator varsEnd() const
				{return m_varlist.end();}
			
		private:
			Cloud& r_cloud;
			const std::map<std::string,std::string>& r_varlist;
			std::map<std::string,std::string> m_varlist;
			std::map<std::string,Solid*> m_volumes;
			std::map<std::string,Aggregate*> m_aggregates;
			std::map<std::string,AggregateNode*> m_nodes;
			std::map<std::string,Vector> m_vectors;
		};
	};

#endif
