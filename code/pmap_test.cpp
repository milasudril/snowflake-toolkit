//@	{
//@	"targets":[{"name":"pmap_test","type":"application","description":"Probability map test"}]
//@	}

#include "pmap_loader.h"
#include "probability_map.h"
#include "randomgenerator.h"

int main()
	{
	SnowflakeModel::RandomGenerator rng;
	auto pmap=SnowflakeModel::pmapLoad("test.png");
	std::vector<size_t> v(pmap.nRowsGet());
	size_t n=1000000;
	while(n!=0)
		{
		auto e=pmap.elementGet(rng);
		++v[e.first];
		--n;
		}

	std::for_each(v.begin(),v.end()
		,[](size_t x)
			{
			printf("%zu\n",x);
			}
		);
	return 0;
	}
