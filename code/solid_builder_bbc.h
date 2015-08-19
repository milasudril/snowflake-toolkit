#ifdef __WAND__
target[name[volume_buildler_bbc.h] type[include]]
dependency[solid_builder_bbc.o]
#endif

#ifndef SNOWFLAKEMODEL_SOLIDBUILDER_BBC_H
#define SNOWFLAKEMODEL_SOLIDBUILDER_BBC_H

#include "grain_visitor.h"
#include "vector.h"
#include <stack>

namespace SnowflakeModel 
	{
	class Solid;
	
	class SolidBuilderBBC:public GrainVisitor
		{
		public:
			SolidBuilderBBC(Solid& mesh_out):
				m_bond{Matrix(),Vector{0,0,0},0,0,0},r_mesh_out(mesh_out)
				{
				}

			void branchBegin(AggregateEdge& edge,Aggregate& aggregate)
				{
				m_bonds.push(m_bond);
				}
				
				
			void branchEnd(AggregateEdge& edge,Aggregate& aggregate)
				{
				m_bond=m_bonds.top();
				m_bonds.pop();	
				}
				
			void grainProcess(AggregateEdge& edge,Aggregate& aggregate);

			~SolidBuilderBBC();

		private:
			struct Bond
				{
				Matrix m_S_parent;
				Vector m_position;
				float m_angle_x;
				float m_angle_y;
				float m_angle_z;
				} m_bond;
			Solid& r_mesh_out;
			std::stack<Bond> m_bonds;
		};
		
	}

#endif
