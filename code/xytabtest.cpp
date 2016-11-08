//@	{
//@	 "targets":
//@		[{
//@		 "name":"xytabtest","type":"application"
//@		,"description":"Test application demonstrating how to load a two-column table into a probability distribution"
//@		}]
//@	}

#include "xytable.h"
#include "file_in.h"
#include "stride_iterator.h"
#include "randomgenerator.h"

#include <map>
#include <iostream>
#include <iomanip>

int main()
	{
	try
		{
		auto table=SnowflakeModel::xytable(SnowflakeModel::FileIn("xytab.txt"));

		SnowflakeModel::StrideIterator<decltype(table)::value_type,0> x(table.data());
		SnowflakeModel::StrideIterator<decltype(table)::value_type,1> y(table.data());

		std::piecewise_linear_distribution<float> pwl(x,x+table.size(),y);

		auto f=std::vector<size_t>(table.size());

		std::map<int, int> hist;
		SnowflakeModel::RandomGenerator randgen;
		for(int n=0; n<100000; ++n)
			{++hist[pwl(randgen)];}

		for(auto p : hist) {
			std::cout << std::setw(2) << std::setfill('0') << p.first << ' '
				<< std::string(p.second/500,'*') << '\n';
		}

		return 0;
		}
	catch(const char* msg)
		{
		fprintf(stderr,"Error: %s\n",msg);
		}

	}
