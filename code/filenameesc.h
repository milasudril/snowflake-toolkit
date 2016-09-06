//@ {
//@	 "targets":[{"name":"filenameesc.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"filenameesc.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_FILENAMEESC_H
#define SNOWFLAKEMODEL_FILENAMEESC_H

#include <string>

namespace SnowflakeModel
	{
	std::string filenameEscape(const char* filename);
	}

#endif
