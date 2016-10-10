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
			static constexpr auto npos=std::numeric_limits<size_t>::max();

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

			size_t offsetGet(const PointInt& pos) const noexcept
				{
				assert(pos.x>=0 && pos.y>=0 && pos.z>=0);
				auto ny=static_cast<size_t>(m_n_y);
				auto nz=static_cast<size_t>(m_n_z);
				if(pos.x<m_n_x && pos.y<m_n_y && pos.z<m_n_z) 
					{return pos.x*ny*nz + pos.y*nz + pos.z;}
				return npos;
				}

			bool cellFilled(size_t offset,uint8_t mask) const noexcept
				{
				return offset==npos?1:m_data[offset]&mask;
				}

			void cellFill(size_t offset,uint8_t mask)noexcept
				{m_data[offset]|=mask;}

			void bitAnd(uint8_t mask) noexcept;

			template<class Callback>
			void pointsVisit(Callback&& cb) const noexcept
				{
				auto nx=m_n_x;
				auto ny=m_n_y;
				auto nz=m_n_z;
				auto pval=m_data;
				for(int k=0;k<nx;++k)
					{
					for(int l=0;l<ny;++l)
						{
						for(int m=0;m<nz;++m)
							{
							cb(k,l,m,*pval);
							++pval;
							}
						}
					}
				}
			
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
