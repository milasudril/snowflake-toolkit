//@ {"targets":[{"name":"xytable.o","type":"object"}]}

#include "xytable.h"
#include "file_in.h"
#include <cstdlib>

using namespace SnowflakeModel;


std::vector<Twins<float>> SnowflakeModel::xytable(FileIn& source)
	{
	int ch_in;
	std::vector<Twins<float>> ret;
	Twins<float> temp;
	auto fieldcount=0;
	std::string buffer;
	while( (ch_in=source.getc())!=EOF )
		{
		switch(fieldcount)
			{
			case 0:
				switch(ch_in)
					{
					case '\t':
						temp.first=atof(buffer.c_str());
						++fieldcount;
						buffer.clear();
						break;
					case '\r':
					case '\n':
						throw "No y value given";
					default:
						buffer+=ch_in;
					}
				break;

			case 1:
				switch(ch_in)
					{
					case '\t':
						temp.first=atof(buffer.c_str());
						++fieldcount;
						buffer.clear();
						break;
					case '\r':
					case '\n':
						temp.second=atof(buffer.c_str());
						ret.push_back(temp);
						buffer.clear();
						break;
					default:
						buffer+=ch_in;
					}
				break;

			default:
				throw "Too many fields in table";
			}
		}
	return std::move(ret);
	}