//@	{"targets":[{"name":"filename.h","type":"include"}]}

#ifndef SNOWFLAKEMODEL_FILENAME_H
#define SNOWFLAKEMODEL_FILENAME_H

#include "alice/typeinfo.hpp"

namespace Alice
	{
	template<>
	struct MakeType<Stringkey("filename")>:public MakeType<Stringkey("string")>
		{
		static constexpr const char* descriptionGet()
			{return "A valid filename";}
		};
	}

#endif
