//@	{"targets":[{"name":"filenameesc.o","type":"object"}]}

#include "filenameesc.h"
#include <algorithm>

static constexpr const char* chars_valid="-.0123456789ABCDEFGHIJKLMNOPQRSTUVXYZ_abcdefghijklmnopqrstuvxyz";
constexpr size_t count(const char* chars)
	{
	return *chars=='\0'?0:1 + count(chars + 1);
	}
static constexpr size_t N_valid=count(chars_valid);

std::string SnowflakeModel::filenameEscape(const char* str)
	{
	std::string ret;
	while(*str!='\0')
		{
		if(std::binary_search(chars_valid,chars_valid + N_valid,*str))
			{ret+=*str;}
		else
			{ret+='_';}
		++str;
		}
	return ret;
	}