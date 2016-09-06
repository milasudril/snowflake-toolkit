//@	{"targets":[{"name":"getdate.o","type":"object"}]}

#include "getdate.h"
#include <ctime>
#include <cstring>

std::string SnowflakeModel::getdate()
	{
	std::string ret;
	ret.resize(31);
	auto now=time(nullptr);
	tm datetime;
	gmtime_r(&now,&datetime);
	strftime(&ret[0],ret.size(),"%F %T",&datetime);
	ret.resize(strlen(ret.data()));
	return ret;
	}