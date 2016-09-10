//@	{"targets":[{"name":"randgentest","type":"application"}]}

#include "randomgenerator.h"
#include <cstdio>
#include <cstring>
#include <cassert>

int main()
	{
	SnowflakeModel::RandomGenerator randgen;

	randgen.seed(0);

	for(size_t k=0;k<1000;++k)
		{
		printf("%u\n",randgen());
		}

	SnowflakeModel::RandomGenerator randgen_2;

	auto x=SnowflakeModel::get(randgen);
	SnowflakeModel::get(randgen_2)=x;

	assert(randgen_2==randgen);

	return 0;
	}
