//@	{"targets":[{"name":"grid.h","type":"include"}]}

#ifndef SNOWFLAKEMODEL_GRID_H
#define SNOWFLAKEMODEL_GRID_H

#include "bounding_box.h"
#include <cmath>
#include <limits>

namespace SnowflakeModel
	{
	class Grid
		{
		public:
			Grid(const BoundingBox& bounding_box
				,double dx,double dy,double dz)
				{
				auto range=bounding_box.m_max - bounding_box.m_min;

				auto nx=static_cast<size_t>( ceil( range.x/dx ) );
				auto ny=static_cast<size_t>( ceil( range.y/dy ) );
				auto nz=static_cast<size_t>( ceil( range.z/dz ) );

				m_n_x=static_cast<int>( nx );
				m_n_y=static_cast<int>( ny );
				m_n_z=static_cast<int>( nz );

				m_data=new uint8_t[nx*ny*nz];
				}

			Grid(unsigned int n_x,unsigned int n_y,unsigned int n_z
				,const BoundingBox& bounding_box):m_n_x(x),m_n_y(y),m_n_z(z)
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

				auto nx=static_cast<size_t>(m_n_x);
				auto ny=static_cast<size_t>(m_n_y);
				auto nz=static_cast<size_t>(m_n_z);

				m_data=new uint8_t[nx*ny*nz];
				}

			~Grid()
				{delete[] m_data;]}

			size_t offsetGet(int x,int y,int z) const noexcept
				{
				assert(x>=0 && y>=0 && z>=0);
				auto nx=static_cast<size_t>(m_n_x);
				auto ny=static_cast<size_t>(m_n_y);
				auto nz=static_cast<size_t>(m_n_z);
				if(x<nx && y<ny && z<nz) 
					{return x*ny*nz + y*nz + z;}
				return std::numeric_limits<size_t>::max();
				}

			bool cellFilled(size_t offset) const noexcept
				{
				return offset==std::numeric_limits<size_t>::max()?
					1:m_data[offset];
				}

			void fill(size_t offset) const noexcept
				{m_data[offset]=1;}

		private:
			int m_n_x;
			int m_n_y;
			int m_n_z;
			uint8_t* m_data;
		};
	};

#endif
