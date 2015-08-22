#ifdef __WAND__
target[name[solid_builder.h] type[include]]
dependency[solid_builder.o]
#endif

#ifndef SNOWFLAKEMODEL_SOLIDBUILDER_H
#define SNOWFLAKEMODEL_SOLIDBUILDER_H

#include "ice_particle_visitor.h"
#include "vector.h"
#include <stack>

namespace SnowflakeModel
	{
	class Solid;

	class SolidBuilder:public IceParticleVisitor
		{
		public:
			SolidBuilder(Solid& mesh_out):
				m_bond{Vector{0,0,0},0,0,0},r_mesh_out(mesh_out)
				{
				}

			void branchBegin(AggregateEdge& edge,AggregateGraph& aggregate)
				{
				m_bonds.push(m_bond);
				}


			void branchEnd(AggregateEdge& edge,AggregateGraph& aggregate)
				{
				m_bond=m_bonds.top();
				m_bonds.pop();
				}

			void iceParticleProcess(AggregateEdge& edge,AggregateGraph& aggregate);

			~SolidBuilder();

		private:
			struct Bond
				{
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
