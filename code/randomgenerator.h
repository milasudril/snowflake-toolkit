//@	{"targets":[{"name":"randomgenerator.h","type":"include"}]}


#ifndef SNOWFLAKEMODEL_RANDOMGENERATOR_H
#define SNOWFLAKEMODEL_RANDOMGENERATOR_H

#include <random>
#include <cstdint>

namespace SnowflakeModel
	{
/**Portable representation of the mersenne twister. GCC uses 64 bit integer
* as its "fast" 32 bit integer. This instanciation always uses uint32_t
* or fails.
*/
	typedef std::mersenne_twister_engine<std::uint32_t, 32, 624, 397, 31,
		0x9908b0df, 11,
		0xffffffff, 7,
        0x9d2c5680, 15,
        0xefc60000, 18, 1812433253> RandomGenerator;

	static_assert(std::is_standard_layout<RandomGenerator>::value
		,"Random generator must have standard layout");

#ifdef __GNUC__
	static constexpr int GNUC=1;
#else
	static constexpr int GNUC=0;
#endif
	static_assert(GNUC==1,"This module requires GCC");

	struct RandomGeneratorTransparent
		{
		static constexpr size_t state_size=624;
		static constexpr size_t size() noexcept
			{return state_size;}
		uint32_t state[state_size];
		size_t position;
		};

	static_assert(sizeof(RandomGenerator)==sizeof(RandomGeneratorTransparent)
		,"Random generator has unexpected size");

	inline RandomGeneratorTransparent& get(RandomGenerator& rng) noexcept 
		{return reinterpret_cast<RandomGeneratorTransparent&>(rng);}

	inline const RandomGeneratorTransparent& get(const RandomGenerator& rng) noexcept
		{return reinterpret_cast<const RandomGeneratorTransparent&>(rng);}
	}

#endif
