//@	{"targets":[{"name":"bvhtreetest","type":"application"}]}

#include "bvhtree.h"

int main()
	{
	SnowflakeModel::BVHTree<int> tree(
		 SnowflakeModel::Vector(10,10,10)
		,4);
	return 0;
	}
