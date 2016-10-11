//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"grid.o",
//@	            "type":"object"
//@	        }
//@	    ]
//@	}
#include "grid.h"
#include "bounding_box.h"
#include <cstring>

using namespace SnowflakeModel;

Grid::Grid(double dx,double dy,double dz,const BoundingBox& bounding_box)
	{
	auto range=bounding_box.m_max - bounding_box.m_min;
	m_n_x=static_cast<int>( ceil( range.x/dx ) );
	m_n_y=static_cast<int>( ceil( range.y/dy ) );
	m_n_z=static_cast<int>( ceil( range.z/dz ) );
	m_dx=dx;
	m_dy=dy;
	m_dz=dz;

	m_data=new uint8_t[m_n_x*m_n_y*m_n_z];

	memset(m_data,0,m_n_x*m_n_y*m_n_z);

	m_min=bounding_box.m_min;
	}

Grid::Grid(unsigned int n_x,unsigned int n_y,unsigned int n_z
	,const BoundingBox& bounding_box):m_n_x(n_x),m_n_y(n_y),m_n_z(n_z)
	{
	if(m_n_x==0)
		{
		if(n_y > 0)
			{
			m_n_x=n_y*(bounding_box.m_max.x -  bounding_box.m_min.x)
				/(bounding_box.m_max.y -  bounding_box.m_min.y)+ 0.5;
			}
		else
		if(n_z > 0)
			{
			m_n_x=n_z*(bounding_box.m_max.x -  bounding_box.m_min.x)
				/(bounding_box.m_max.z -  bounding_box.m_min.z)+ 0.5;
			}
		else
			{throw "At leas one of Ny and Nz must be grater than zero";}
		}
	
	if(m_n_y==0)
		{
		if(n_x > 0)
			{
			m_n_y=n_x*(bounding_box.m_max.y -  bounding_box.m_min.y)
				/(bounding_box.m_max.x -  bounding_box.m_min.x)+ 0.5;
			}
		else
		if(n_z > 0)
			{
			m_n_y=n_z*(bounding_box.m_max.y -  bounding_box.m_min.y)
				/(bounding_box.m_max.z -  bounding_box.m_min.z)+ 0.5;
			}
		else
			{throw "At leas one of Nx and Nz must be grater than zero";}
		}
	
	if(m_n_z==0)
		{
		if(n_x > 0)
			{
			m_n_z=n_x*(bounding_box.m_max.z -  bounding_box.m_min.z)
				/(bounding_box.m_max.x -  bounding_box.m_min.x)+ 0.5;
			}
		else
		if(n_y > 0)
			{
			m_n_z=n_y*(bounding_box.m_max.z -  bounding_box.m_min.z)
				/(bounding_box.m_max.y -  bounding_box.m_min.y)+ 0.5;
			}
		else
			{throw "At leas one of Nx and Ny must be grater than zero";}
		}
	
	if(m_n_x==0 || m_n_y==0 || m_n_z==0)
		{throw "More points are needed to sample the generated geometry";}
	
	m_data=new uint8_t[m_n_x*m_n_y*m_n_z];
	
	memset(m_data,0,m_n_x*m_n_y*m_n_z);

	auto range=bounding_box.m_max - bounding_box.m_min;
	m_dx=range.x/m_n_x;
	m_dy=range.y/m_n_y;
	m_dz=range.z/m_n_z;

	m_min=bounding_box.m_min;
	}

Grid::~Grid()
	{
	delete[] m_data;
	}

void Grid::bitAnd(size_t offset_start,size_t offset_end,uint8_t mask) noexcept
	{
//	TODO: make this faster (fill a vectorized type with mask);
	auto ptr=m_data + offset_start;
	auto ptr_end=m_data + offset_end;
	while(ptr!=ptr_end)
		{
		*ptr&=mask;
		++ptr;
		}
	}
