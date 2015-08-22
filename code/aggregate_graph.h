#ifdef __WAND__
target[name[aggregate_graph.h] type[include]]
dependency[aggregate_graph.o]
#endif

#ifndef SNOWFLAKEMODEL_AGGREGATEGRAPH_H
#define SNOWFLAKEMODEL_AGGREGATEGRAPH_H

#include "aggregate_node.h"
#include <memory>
#include <cstddef>

namespace SnowflakeModel
	{
	class IceParticleVisitor;

	class AggregateGraph
		{
		public:
			AggregateGraph():m_position(0,0,0)
				,m_angle_root_x(0),m_angle_root_y(0),m_angle_root_z(0)
				{}

			void nodesVisit(IceParticleVisitor&& builder);

			AggregateNode& nodeAppend(const Vector& u
				,AggregateNode& node_v,const Vector& v
				,float angle_x,float angle_y,float angle_z)
				{
				m_nodes.push_back(std::unique_ptr<AggregateNode>{new AggregateNode});
				auto& x=*(m_nodes.back().get());

				bondCreate(x,u,node_v,v,angle_x,angle_y,angle_z);
				return x;
				}

			AggregateNode& nodeAppend()
				{
				m_nodes.push_back(std::unique_ptr<AggregateNode>{new AggregateNode});
				return *(m_nodes.back().get());
				}

			const Vector& positionGet() const noexcept
				{return m_position;}

		private:
			std::vector< std::unique_ptr<AggregateNode> > m_nodes;
			Vector m_position;
			float m_angle_root_x;
			float m_angle_root_y;
			float m_angle_root_z;
		};
	}

#endif
