#ifdef __WAND__
target[name[voxelbuilder_adda.h] type[include]]
dependency[voxelbuilder_adda.o]
#endif

#ifndef SNOWFLAKEMODEL_VOXELBUILDERADDA_H
#define SNOWFLAKEMODEL_VOXELBUILDERADDA_H

#include "voxel_builder.h"
#include "bounding_box.h"

namespace SnowflakeModel
	{
	class FileOut;	
	
	class VoxelbuilderAdda:public VoxelBuilder
		{
		public:
			VoxelbuilderAdda(FileOut& dest,int n_x,int n_y,int n_z
				,const BoundingBox& bounding_box);
			
			~VoxelbuilderAdda();
			bool fill(const PointInt& v);
			void volumeStart(const VolumeConvex& volume_next);
			PointInt quantize(const Point& p) const;
			Point dequantize(const PointInt& p) const;
			
		private:
			FileOut& r_dest;
			uint8_t* m_data_filled;
			uint8_t* m_data_stop;
			BoundingBox m_bounding_box;
			const VolumeConvex* r_volume_current;
			int m_n_x;
			int m_n_y;
			int m_n_z;
		};
	}

#endif
