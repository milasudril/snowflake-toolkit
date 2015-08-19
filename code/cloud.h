#ifdef __WAND__
target[name[cloud.h] type[include]]
#endif

#ifndef SNOWFLAKEMODEL_CLOUD_H
#define SNOWFLAKEMODEL_CLOUD_H

#include "aggregate.h"
#include "solid.h"
#include <list>

namespace SnowflakeModel
	{
	class Cloud
		{
		public:
			Solid* solidAppend(Solid&& solid)
				{
				m_solids.push_back(std::move(solid));
				return &m_solids.back();
				}
				
			Aggregate* aggregateAppend(Aggregate&& aggregate)
				{
				m_aggregates.push_back(std::move(aggregate));
				return &m_aggregates.back();
				}
				
			AggregateNode* nodeAppend(AggregateNode&& node)
				{
				m_nodes.push_back(std::move(node));
				return &m_nodes.back();
				}
				
			std::list<Aggregate>::const_iterator aggregatesBegin() const
				{return m_aggregates.begin();}
			
			std::list<Aggregate>::const_iterator aggregatesEnd() const
				{return m_aggregates.end();}
				
			std::list<Aggregate>::iterator aggregatesBegin()
				{return m_aggregates.begin();}
			
			std::list<Aggregate>::iterator aggregatesEnd()
				{return m_aggregates.end();}

		private:
			std::list<Solid> m_solids;
			std::list<Aggregate> m_aggregates;
			std::list<AggregateNode> m_nodes;
		};
	}

#endif
