//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"solid_builder_bbc.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "solid_builder_bbc.h"
#include "aggregate_node.h"
#include "solid.h"

using namespace SnowflakeModel;


SolidBuilderBBC::~SolidBuilderBBC()
	{
	assert(m_bonds.size()==0);
	}

void SolidBuilderBBC::iceParticleProcess(AggregateEdge& edge,AggregateGraph& aggregate)
	{
//	O + r_1 = P_1
//	P_1 + v = B
//	O + r_0 = P_0
//	P_0 + u = A
//	A=B
//
//	r_1 - r_0 =P_1 -  P_0 = B - v - (A -u) = - v + u
//	r_1 = r_0 + u - v
//	u is offset_parent
//	v    offset_child
//	r    position
//
//	U=R(\theta)*u
//	V=R(\theta+\Delta\theta)*v

	Matrix R_0;
	R_0=glm::rotate(R_0,m_bond.m_angle_z,{0,0,1});
	R_0=glm::rotate(R_0,m_bond.m_angle_y,{0,1,0});
	R_0=glm::rotate(R_0,m_bond.m_angle_x,{1,0,0});

	auto R=glm::rotate(R_0,edge.m_angle_z,{0,0,1});
	R=glm::rotate(R,edge.m_angle_y,{0,1,0});
	R=glm::rotate(R,edge.m_angle_x,{1,0,0});

	auto& sol_temp=edge.r_node_child->iceParticleGet().solidGet();
	Matrix S_child;
	if(sol_temp.subvolumesCount()!=0)
		{
		auto& bb_temp=sol_temp.boundingBoxGet();
		S_child=glm::scale(S_child,Vector(bb_temp.m_max-bb_temp.centerGet()));
		}
	else
		{S_child=glm::scale(S_child,{0.0f,0.0f,0.0f});}

	m_bond.m_position+=
		Vector(
			R_0*m_bond.m_S_parent*Point(edge.m_offset_parent,1)
			-R*S_child*Point(edge.m_offset_child,1)
		);


	m_bond.m_angle_x+=edge.m_angle_x;
	m_bond.m_angle_y+=edge.m_angle_y;
	m_bond.m_angle_z+=edge.m_angle_z;
	m_bond.m_S_parent=S_child;

	Matrix T;
	T=glm::translate(T,m_bond.m_position);
	T=glm::rotate(T,m_bond.m_angle_z,Vector{0,0,1});
	T=glm::rotate(T,m_bond.m_angle_y,Vector{0,1,0});
	T=glm::rotate(T,m_bond.m_angle_x,Vector{1,0,0});

	r_mesh_out.merge(T,sol_temp,0);
	}
