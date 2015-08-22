#ifdef __WAND__
target[name[aggregate_graph.o] type[object]]
#endif

#include "aggregate_graph.h"
#include "ice_particle_visitor.h"
#include "twins.h"
#include <stack>

#include <cstdio>

using namespace SnowflakeModel;

namespace
	{
	void nodesVisit(AggregateEdge& edge_current,IceParticleVisitor& visitor
		,unsigned int color_prev
		,AggregateGraph& aggregate
		,unsigned int recursion_depth)
		{
		auto node_current=edge_current.r_node_child;
		if(node_current->colorGet()==color_prev)
			{
			if(recursion_depth < 64)
				{
				visitor.branchBegin(edge_current,aggregate);

				visitor.iceParticleProcess(edge_current,aggregate);
				node_current->colorToggle();
				auto ptr_begin=node_current->bondsBegin();
				auto ptr_end=node_current->bondsEnd();
				while(ptr_begin!=ptr_end)
					{
					nodesVisit(*ptr_begin,visitor,color_prev,aggregate
						,recursion_depth+1);
					++ptr_begin;
					}

				visitor.branchEnd(edge_current,aggregate);
				}
			else
				{
			//	nodesVisit(...)

				}
			}
		}
	struct GrainsVisitStackFrame
		{
		AggregateEdge* current;
		AggregateEdge* visit;
		AggregateEdge* end;
		};

	void nodesVisit(AggregateEdge& e_root
		,IceParticleVisitor& visitor
		,unsigned int color_prev
		,AggregateGraph& aggregate)
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
		visitor.iceParticleProcess(e_root,aggregate);

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
					visitor.iceParticleProcess(*edge_visit,aggregate);
					}
				}
			}
		}
	}

void AggregateGraph::nodesVisit(IceParticleVisitor&& visitor)
	{
	assert(m_nodes.size()!=0);

	auto ptr=m_nodes.begin();
	auto end=m_nodes.end();
	auto color_init=(*ptr)->colorGet();
	while(ptr!=end)
		{
		if((*ptr)->colorGet()==color_init)
			{
			AggregateEdge e_root
				{
				nullptr,m_position,ptr->get(),Vector{0,0,0}
				,m_angle_root_x
				,m_angle_root_y
				,m_angle_root_z
				};
			::nodesVisit(e_root,visitor,(*ptr)->colorGet(),*this);
			}
		++ptr;
		}
	}
