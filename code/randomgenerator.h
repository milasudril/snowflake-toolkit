//@	{"targets":[{"name":"randomgenerator.h","type":"include"}]}


#ifndef SNOWFLAKEMODEL_RANDOMGENERATOR_H
#define SNOWFLAKEMODEL_RANDOMGENERATOR_H
#include <random>
#include <cstdint>

namespace SnowflakeModel
	{
/**Portable representation of the mersenne twister. GCC uses 64 bit integer
* as its "fast" 32 bit integer. This instanciation always uses uint32_t
* of fails.
*/
	typedef std::mersenne_twister_engine<std::uint32_t, 32, 624, 397, 31,
		0x9908b0df, 11,
		0xffffffff, 7,
        0x9d2c5680, 15,
        0xefc60000, 18, 1812433253> RandomGenerator;

	static_assert(std::is_standard_layout<RandomGenerator>::value
		,"Random generator must have standard layout");

/**State accessors needed for binary I/O
*/
	inline const uint32_t* begin(const RandomGenerator& rng)
		{return reinterpret_cast<const uint32_t*>(&rng);}

	inline const uint32_t* end(const RandomGenerator& rng)
		{return begin(rng) + 624;}

	inline uint32_t* begin(RandomGenerator& rng)
		{return reinterpret_cast<uint32_t*>(&rng);}

	inline uint32_t* end(RandomGenerator& rng)
		{return begin(rng) + 624;}

	inline constexpr size_t length(const RandomGenerator& rng)
		{return 624;}
	
	}


#endif