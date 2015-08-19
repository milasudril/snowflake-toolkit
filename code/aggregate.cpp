#ifdef __WAND__
target[name[aggregate.o] type[object]]
#endif

#include "aggregate.h"
#include "grain_visitor.h"
#include "twins.h"
#include <stack>

#include <cstdio>

using namespace SnowflakeModel;

namespace
	{
	void grainsVisit(AggregateEdge& edge_current,GrainVisitor& visitor
		,unsigned int color_prev
		,Aggregate& aggregate
		,unsigned int recursion_depth)
		{
		auto node_current=edge_current.r_node_child;
		if(node_current->colorGet()==color_prev)
			{
			if(recursion_depth < 64)
				{	
				visitor.branchBegin(edge_current,aggregate);

				visitor.grainProcess(edge_current,aggregate);
				node_current->colorToggle();
				auto ptr_begin=node_current->bondsBegin();
				auto ptr_end=node_current->bondsEnd();
				while(ptr_begin!=ptr_end)
					{
					grainsVisit(*ptr_begin,visitor,color_prev,aggregate
						,recursion_depth+1);
					++ptr_begin;
					}
	
				visitor.branchEnd(edge_current,aggregate);
				}
			else
				{
			//	grainsVisit(...)
					
				}
			}
		}
	struct GrainsVisitStackFrame
		{
		AggregateEdge* current;
		AggregateEdge* visit;
		AggregateEdge* end;
		};
		
	void grainsVisit(AggregateEdge& e_root
		,GrainVisitor& visitor
		,unsigned int color_prev
		,Aggregate& aggregate)
		{
		std::stack< GrainsVisitStackFrame > history;
		history.push(
			{
			&e_root
			,e_root.r_node_child->bondsBegin()
			,e_root.r_node_child->bondsEnd()
			});
		
		visitor.branchBegin(e_root,aggregate);
		e_root.r_node_child->colorToggle();
		visitor.grainProcess(e_root,aggregate);

		while(history.size())
			{
			auto edges=history.top();
			history.pop();
			auto edge_current=edges.current;
			auto edge_visit=edges.visit;
		
			if(edge_visit==edges.end)
				{visitor.branchEnd(*edge_current,aggregate);}
			else
				{
				history.push({edge_current,edge_visit+1,edges.end});
				auto node_visit=edge_visit->r_node_child;
				if(node_visit->colorGet()==color_prev)
					{
					history.push(
						{
						edge_visit
						,node_visit->bondsBegin()
						,node_visit->bondsEnd()
						});
					
					visitor.branchBegin(*edge_visit,aggregate);
					node_visit->colorToggle();
					visitor.grainProcess(*edge_visit,aggregate);
					}
				}
			}
		}
	}

void Aggregate::grainsVisit(GrainVisitor&& visitor)
	{
	AggregateEdge e_root
		{
		 nullptr,m_position,&m_root,Vector{0,0,0}
		,m_angle_root_x
		,m_angle_root_y
		,m_angle_root_z
		};
	::grainsVisit(e_root,visitor,m_root.colorGet(),*this);
	}
