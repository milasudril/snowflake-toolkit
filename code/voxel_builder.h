//@	{
//@	    "dependencies_extra":[],
//@	    "targets":[
//@	        {
//@	            "dependencies":[],
//@	            "name":"voxel_builder.h",
//@	            "type":"include"
//@	        }
//@	    ]
//@	}
#ifndef SNOWFLAKEMODEL_VOXELBUILDER_H
#define SNOWFLAKEMODEL_VOXELBUILDER_H

#include "vector.h"

namespace SnowflakeModel
	{
	class VolumeConvex;
	class BoundingBox;
	class VoxelBuilder
		{
		public:
			virtual void fill(const PointInt& position)=0;
			virtual PointInt quantize(const Point& position) const=0;
			virtual Point dequantize(const PointInt& position) const=0;
			virtual VectorInt gridSizeGet() const noexcept=0;
		};
		
	}

#endif
