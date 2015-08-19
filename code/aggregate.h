#ifdef __WAND__
target[name[aggregate.h] type[include]]
dependency[aggregate.o]
#endif

#ifndef SNOWFLAKEMODEL_AGGREGATE_H
#define SNOWFLAKEMODEL_AGGREGATE_H

#include "aggregate_node.h"

#include <cstddef>

namespace SnowflakeModel
	{
	class GrainVisitor;
	
	/**\brief An Aggregate consists of a set of nodes, building the shape of a
	 * snowflake
	*/
	class Aggregate
		{
		public:
			Aggregate():m_position(0,0,0)
				,m_angle_root_x(0),m_angle_root_y(0),m_angle_root_z(0)
				{}
			
			void grainsVisit(GrainVisitor&& builder);

			Aggregate& childAppend(const Vector& u
				,AggregateNode& node_v,const Vector& v
				,float angle_x,float angle_y,float angle_z)
				{
				bondCreate(m_root,u,node_v,v,angle_x,angle_y,angle_z);
				return *this;
				}
			
			AggregateNode& rootGet()
				{return m_root;}
				
			const Vector& positionGet() const
				{return m_position;}

		private:
			AggregateNode m_root;
			Vector m_position;
			float m_angle_root_x;
			float m_angle_root_y;
			float m_angle_root_z;
		};
	}

#endif
