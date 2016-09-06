//@ {
//@	 "targets":[{"name":"getdate.h","type":"include"}]
//@	,"dependencies_extra":[{"ref":"getdate.o","rel":"implementation"}]
//@	}

#ifndef SNOWFLAKEMODEL_GETDATE_H
#define SNOWFLAKEMODEL_GETDATE_H

#include <string>

namespace SnowflakeModel
	{
	std::string getdate();
	}

#endif
