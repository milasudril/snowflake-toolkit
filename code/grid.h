//@	{
//@	    "dependencies_extra":[
//@	        {
//@	            "ref":"grid.o",
//@	            "rel":"implementation"
//@	        }
//@	    ],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"grid.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_GRID_H
#define SNOWFLAKEMODEL_GRID_H

#include "vector.h"

namespace SnowflakeModel
	{
	class BoundingBox;
	class Grid
		{
		public:
			Grid(unsigned int n_x,unsigned int n_y,unsigned int n_z
				,const BoundingBox& bounding_box);

			Grid(double dx,double dy,double dz,const BoundingBox& bounding_box);
			
			~Grid();

			PointInt quantize(const Point& p) const
				{
				auto pos=p-m_min;
			//	TODO (perf): Vectorize?	
				return PointInt
					{
					 pos.x/m_dx
					,pos.y/m_dy
					,pos.z/m_dz
					,1
					};
				}
	
			Point dequantize(const PointInt& p) const
				{
			
//	TODO (perf): Vectorize?	
				return Point
					{
					 m_min.x + (p.x+0.5)*m_dx
					,m_min.y + (p.y+0.5)*m_dy
					,m_min.z + (p.z+0.5)*m_dz
					,1
					};
				}

			size_t offsetGet(int x,int y,int z) const noexcept
				{
				assert(x>=0 && y>=0 && z>=0);
				auto ny=static_cast<size_t>(m_n_y);
				auto nz=static_cast<size_t>(m_n_z);
				if(x<m_n_x && y<m_n_y && z<m_n_z) 
					{return x*ny*nz + y*nz + z;}
				return std::numeric_limits<size_t>::max();
				}

			bool cellFilled(size_t offset) const noexcept
				{
				return offset==std::numeric_limits<size_t>::max()?
					1:m_data[offset];
				}

			void fill(size_t offset,uint8_t mask) const noexcept
				{m_data[offset]|=mask;}

			void dataClear(uint8_t mask) noexcept;
			
		private:
			uint8_t* m_data;
			double m_dx;
			double m_dy;
			double m_dz;
			Point m_min;
			int m_n_x;
			int m_n_y;
			int m_n_z;
		};
	}

#endif
