//@	{"targets":[{"name":"grid_definition2.h","type":"include"}]}

#ifndef SNOWFLAKEMODEL_GRIDDEFINTION2_H
#define SNOWflAKEMODEL_GRIDDEFINTION2_H

#include "alice/stringkey.hpp"
#include "alice/typeinfo.hpp"

namespace SnowflakeModel
	{
	struct GridDefinition2
		{
		size_t N;
		double r_x;
		double r_y;
		double r_z;
		std::string filename;
		};
	}

namespace Alice
	{
	template<>
	struct MakeType<Stringkey("Grid definition 2")>
		{
		typedef SnowflakeModel::GridDefinition Type;
		};
	}

#endif
