#ifdef __WAND__
target[name[aggregate_graph_loader.h] type[include]]
dependency[aggregate_graph_loader.o]
#endif

#ifndef SNOWFLAKEMODEL_AGGREGATEGRAPHLOADER_H
#define SNOWFLAKEMODEL_AGGREGATEGRAPHLOADER_H

#include "config_commandhandler.h"
#include "vector.h"

#include <map>

namespace SnowflakeModel
	{
	class Solid;
	class AggregateGraph;
	class AggregateNode;

	class AggregateGraphLoader:public ConfigCommandHandler
		{
		public:
			AggregateGraphLoader(AggregateGraph& graph
				,const std::map<std::string,std::string>& varlist
				,std::map<std::string,Solid>& solids)
				:r_graph(graph),r_varlist(varlist),r_solids(solids)
				{}

			std::string invoke(const ConfigCommand& invoke,const FileIn& source);

			std::map<std::string,std::string>::const_iterator varsBegin() const
				{return m_varlist.begin();}

			std::map<std::string,std::string>::const_iterator varsEnd() const
				{return m_varlist.end();}

		private:
			AggregateGraph& r_graph;
			const std::map<std::string,std::string>& r_varlist;
			std::map<std::string,Solid>& r_solids;
			std::map<std::string,std::string> m_varlist;
			std::map<std::string,AggregateNode*> m_nodes;
			std::map<std::string,Vector> m_vectors;
		};
	};

#endif
