#ifdef __WAND__
target[name[bounding_box.h] type[include]]
#endif

#ifndef SNOWFLAKEMODEL_BOUNDINGBOX_H
#define SNOWFLAKEMODEL_BOUNDINGBOX_H

#include "vector.h"

namespace SnowflakeModel
	{
	struct BoundingBox
		{
		Vector m_min;
		Vector m_max;
		};
	}

#endif
