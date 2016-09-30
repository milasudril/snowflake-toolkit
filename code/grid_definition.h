//@	{
//@	 "targets":[{"name":"grid_definition.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"grid_definition.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_GRIDDEFINTION_H
#define SNOWflAKEMODEL_GRIDDEFINTION_H

#include "alice/stringkey.hpp"
#include "alice/typeinfo.hpp"

namespace SnowflakeModel
	{
	struct GridDefinition
		{
		int N_x;
		int N_y;
		int N_z;
		std::string filename;
		};
	}

namespace Alice
	{
	template<>
	struct MakeType<Stringkey("Grid definition")>
		{
		typedef SnowflakeModel::GridDefinition Type;
		};
	}

#endif
