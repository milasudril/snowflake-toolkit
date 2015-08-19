#ifdef __WAND__
target[name[aggregate_node.o] type[object]]
#endif

#include "aggregate_node.h"
#include "twins.h"

using namespace SnowflakeModel;

void SnowflakeModel::bondCreate(AggregateNode& node_u,const Vector& u
	,AggregateNode& node_v,const Vector& v
	,float angle_x,float angle_y,float angle_z)
	{
	Twins<AggregateEdge> temp
		{
			 {&node_u,u,&node_v,v, angle_x, angle_y, angle_z}
			,{&node_v,v,&node_u,u,-angle_z,-angle_y,-angle_x}
		};

	node_u.m_bonds.push_back(temp.first);
	node_v.m_bonds.push_back(temp.second);
	}
