#ifdef __WAND__
target[name[aggregate_node.h] type[include]]
dependency[aggregate_node.o]
#endif

#ifndef SNOWFLAKEMODEL_AGGREGATENODE_H
#define SNOWFLAKEMODEL_AGGREGATENODE_H

#include "grain.h"
#include "aggregate_edge.h"
#include <vector>

namespace SnowflakeModel
	{
	void bondCreate(AggregateNode& node_u,const Vector& u
		,AggregateNode& node_v,const Vector& v
		,float angle_x,float angle_y,float angle_z);
	
	/**\brief An AggregateNode stores geometric and physical information about the
	 * ice crystal forming the node. It also stores the all bonds to its "child"
	 * nodes.
	*/
	class AggregateNode
		{
		public:
			AggregateNode():m_flags(0){}
		
			bool leafIs() const
				{return m_bonds.size()<2;}
			
			unsigned int colorGet() const
				{return m_flags&COLOR_BIT;}
			
			void colorToggle()
				{m_flags^=COLOR_BIT;}

			Grain& grainGet()
				{return m_grain;}
				
			AggregateEdge* bondsBegin()
				{return m_bonds.data();}
			
			AggregateEdge* bondsEnd()
				{return m_bonds.data()+m_bonds.size();}
		
		private:
			friend void bondCreate(AggregateNode& node_u,const Vector& u
				,AggregateNode& node_v,const Vector& v
				,float angle_x,float angle_y,float angle_z);
			
			Grain m_grain;
			std::vector<AggregateEdge> m_bonds;
			static constexpr unsigned int COLOR_BIT=1;
			unsigned int m_flags;
		};
	}

#endif

